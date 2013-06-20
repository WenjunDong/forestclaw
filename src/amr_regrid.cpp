/*
Copyright (c) 2012 Carsten Burstedde, Donna Calhoun
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "amr_forestclaw.H"
#include "amr_utils.H"

/* -----------------------------------------------------------------
   Regridding
     - Initialization routines
     - cell tagging
     - interpolating/coarsening as needed
   ---------------------------------------------------------------- */

void cb_tag4refinement(fclaw2d_domain_t *domain,
                       fclaw2d_patch_t *this_patch,
                       int this_block_idx,
                       int this_patch_idx,
                       void *user)
{
    fclaw_bool init_flag = *((fclaw_bool *) user);
    const amr_options_t *gparms = get_domain_parms(domain);
    int maxlevel = gparms->maxlevel;
    fclaw_bool patch_refined = false;

    // ClawPatch *cp = get_clawpatch(this_patch);

    int level = this_patch->level;
    set_debug_info_(this_block_idx, this_patch_idx, level);

    if (level < maxlevel)
    {
        int initflag = init_flag ? 1 : 0;
        fclaw2d_domain_data_t* ddata = get_domain_data(domain);

        patch_refined =
            (ddata->f_patch_tag4refinement)(domain,this_patch,this_block_idx,
                                            this_patch_idx,initflag);

        // patch_refined = cp->tag_for_refinement(init_flag);
        if (patch_refined)
        {
            fclaw2d_patch_mark_refine(domain, this_block_idx, this_patch_idx);
        }
    }
}

fclaw_bool patch_tag4refinement_default(fclaw2d_domain_t *domain,
                                        fclaw2d_patch_t *this_patch,
                                        int this_block_idx, int this_patch_idx,
                                        int initflag)
{
    /* ----------------------------------------------------------- */
    // Global parameters
    const amr_options_t *gparms = get_domain_parms(domain);
    int mx = gparms->mx;
    int my = gparms->my;
    int mbc = gparms->mbc;
    int meqn = gparms->meqn;

    /* ----------------------------------------------------------- */
    // Patch specific parameters
    ClawPatch *cp = get_clawpatch(this_patch);
    double xlower = cp->xlower();
    double ylower = cp->ylower();
    double dx = cp->dx();
    double dy = cp->dy();

    /* ------------------------------------------------------------ */
    // Pointers needed to pass to Fortran
    double* q = cp->q();

    int tag_patch;  // == 0 or 1
    tag4refinement_(mx,my,mbc,meqn,xlower,ylower,
                    dx, dy,q,initflag,tag_patch);
    return tag_patch == 1;
}

/* Tag family for coarsening */
static
void cb_tag4coarsening(fclaw2d_domain_t *domain,
                               fclaw2d_patch_t *sibling_patch,
                               int this_block_idx,
                               int sibling0_patch_idx,
                               void *user)
{
    const amr_options_t *gparms = get_domain_parms(domain);
    int minlevel = gparms->minlevel;
    // const int p4est_refineFactor = get_p4est_refineFactor(domain);

    int level = sibling_patch[0].level;

    // Make this information accessible to Fortran
    set_debug_info_(this_block_idx, sibling0_patch_idx, level);

    if (level > minlevel)
    {
        int refratio = gparms->refratio;
        fclaw_bool patch_coarsened = false;

        ClawPatch *cp_new_coarse = new ClawPatch();

        // Create a new coarse grid patch that contains the siblings;
        // Average siblings to this patch and test whether coarse
        // patch should be refined.  If not, then we coarsen the
        // sibling "family" of patches.
        cp_new_coarse->define(sibling_patch[0].xlower,
                              sibling_patch[0].ylower,
                              sibling_patch[NumSiblings-1].xupper,
                              sibling_patch[NumSiblings-1].yupper,
                              this_block_idx,
                              level,
                              gparms);

        // cp_new_coarse->setup_patch(level, maxlevel, refratio);
        // Create a fake patch? Don't set up the patch?
        // set_user_patch_setup(new_domain,&new_patch[igrid],blockno,new_patchno);

        ClawPatch *cp_siblings[NumSiblings];
        for (int i = 0; i < NumSiblings; i++)
        {
            cp_siblings[i] = get_clawpatch(&sibling_patch[i]);
        }

        // Pass all four sibling patches into a single routine to see if
        // they can be coarsened.

        cp_new_coarse->coarsen_from_fine_family(cp_siblings,refratio,NumSiblings,
                                                p4est_refineFactor);

        fclaw2d_domain_data_t* ddata = get_domain_data(domain);
        patch_coarsened =
            (ddata->f_patch_tag4coarsening)(domain,sibling_patch,this_block_idx,
                                            sibling0_patch_idx,cp_new_coarse);

        /*
        fclaw_bool pc1;
        pc1 = cp_new_coarse->tag_for_coarsening(cp_siblings,refratio,
                                                NumSiblings,
                                                p4est_refineFactor);

        if (pc1 != patch_coarsened)
        {
            printf("Different coarsening\n");
        }
        */


        if (patch_coarsened)
        {
            for (int i = 0; i < NumSiblings; i++)
            {
                int sibling_patch_idx = sibling0_patch_idx + i;
                fclaw2d_patch_mark_coarsen(domain, this_block_idx, sibling_patch_idx);
            }
        }
        delete cp_new_coarse;
    }
}

fclaw_bool patch_tag4coarsening_default(fclaw2d_domain_t *domain,
                                        fclaw2d_patch_t *sibling_patch,
                                        int this_block_idx,
                                        int sibling0_patch_idx,
                                        ClawPatch* cp_new_coarse)
{
    /* ----------------------------------------------------------- */
    // Global parameters
    const amr_options_t *gparms = get_domain_parms(domain);
    int mx = gparms->mx;
    int my = gparms->my;
    int mbc = gparms->mbc;
    int meqn = gparms->meqn;

    /* ----------------------------------------------------------- */
    // Patch specific parameters
    ClawPatch *cp = cp_new_coarse;
    double xlower = cp->xlower();
    double ylower = cp->ylower();
    double dx = cp->dx();
    double dy = cp->dy();

    /* ------------------------------------------------------------ */
    // Pointers needed to pass to Fortran
    double* qcoarse = cp->q();

    int tag_patch;  // == 0 or 1
    tag4coarsening_(mx,my,mbc,meqn,xlower,ylower,
                    dx,dy,qcoarse,tag_patch);
    return tag_patch == 0;
}


void cb_domain_adapt(fclaw2d_domain_t * old_domain,
                     fclaw2d_patch_t * old_patch,
                     fclaw2d_domain_t * new_domain,
                     fclaw2d_patch_t * new_patch,
                     fclaw2d_patch_relation_t newsize,
                     int blockno,
                     int old_patchno, int new_patchno,
                     void *user)
{

    const amr_options_t *gparms = get_domain_parms(old_domain);

    /* const int num_siblings = get_siblings_per_patch(old_domain); */
    fclaw_bool init_grid = *(fclaw_bool *) user;

    // const int p4est_refineFactor = get_p4est_refineFactor(old_domain);
    int refratio = gparms->refratio;

    if (newsize == FCLAW2D_PATCH_SAMESIZE)
    {
        // Grid was not coarsened or refined, so we can just copy
        // the pointer
        ClawPatch *cp_old = get_clawpatch(&old_patch[0]);
        fclaw_bool old_code = fclaw_true;
        if (old_code)
        {
            // This produces what appear to be correct results
            ClawPatch *cp_new = new ClawPatch();
            int level = new_patch->level;
            cp_new->define(old_patch->xlower,
                           old_patch->ylower,
                           old_patch->xupper,
                           old_patch->yupper,
                           blockno,
                           level,
                           gparms);

            set_patch_data(&new_patch[0],cp_new);

            fclaw2d_solver_functions_t *sf = get_solver_functions(old_domain);
            (sf->f_patch_setup)(new_domain,&new_patch[0],blockno,new_patchno);

            cp_new->copyFrom(cp_old);
        }
        else
        {
            // whereas this version has memory problems.
            set_patch_data(&new_patch[0],cp_old);
            set_patch_data(&old_patch[0],NULL);
        }
    }
    else if (newsize == FCLAW2D_PATCH_HALFSIZE)
    {
        // New grids are FINER grids
        ClawPatch *cp_old = get_clawpatch(&old_patch[0]);

        for (int igrid = 0; igrid < NumSiblings; igrid++)
        {
            ClawPatch *cp_new = new ClawPatch();
            int level = new_patch->level;

            cp_new->define(new_patch[igrid].xlower,
                           new_patch[igrid].ylower,
                           new_patch[igrid].xupper,
                           new_patch[igrid].yupper,
                           blockno,
                           level,
                           gparms);

            set_patch_data(&new_patch[igrid],cp_new);
            fclaw2d_solver_functions_t *sf = get_solver_functions(old_domain);

            (sf->f_patch_setup)(new_domain,&new_patch[igrid],blockno,new_patchno);

            // int level = new_patch[igrid].level;
            // cp_new->setup_patch(level, maxlevel, refratio);
            if (init_grid)
            {
                (sf->f_patch_initialize)(new_domain,&new_patch[igrid],blockno,new_patchno);
            }
            else
            {
                cp_old->interpolate_to_fine_patch(cp_new,igrid,p4est_refineFactor,refratio);
            }
        }
    }
    else if (newsize == FCLAW2D_PATCH_DOUBLESIZE)
    {
        // newsize == DOUBLESIZE (must remember  : DOUBLESIZE means a coarser grid!)
        // new grid is a COARSE grid
        ClawPatch *cp_new = new ClawPatch();
        int level = new_patch->level;
        cp_new->define(new_patch[0].xlower,
                       new_patch[0].ylower,
                       new_patch[0].xupper,
                       new_patch[0].yupper,
                       blockno,
                       level,
                       gparms);
        set_patch_data(&new_patch[0],cp_new);

        fclaw2d_solver_functions_t *sf = get_solver_functions(old_domain);
        (sf->f_patch_setup)(new_domain,&new_patch[0],blockno,new_patchno);

        /*
        int level = new_patch[0].level;
        cp_new->setup_patch(level, maxlevel, refratio);
        */

        ClawPatch *cp_siblings[NumSiblings]; // An array of pointers?
        for (int i = 0; i < NumSiblings; i++)
        {
            cp_siblings[i] = get_clawpatch(&old_patch[i]);
        }
        // This duplicates the work we did to determine if we even need to coarsen. Oh well.
        cp_new->coarsen_from_fine_family(cp_siblings, refratio, NumSiblings,
                                         p4est_refineFactor);
    }
    else
    {
        printf("cb_adapt_domain : newsize not recognized\n");
        exit(1);
    }
}


void regrid(fclaw2d_domain_t **domain)
{

    const amr_options_t *gparms = get_domain_parms(*domain);
    double t = get_domain_time(*domain);

    fclaw_bool init_flag = false;

    int minlevel = gparms->minlevel;
    int maxlevel = gparms->maxlevel;

    for(int level = maxlevel; level > minlevel; level--)
    {
        double alpha = 0;
        exchange_with_coarse(*domain,level,t,alpha);
    }

    // First determine which families should be coarsened.
    fclaw2d_domain_iterate_families(*domain, cb_tag4coarsening,
                                    (void*) NULL);

    // Then refine.
    fclaw2d_domain_iterate_patches(*domain, cb_tag4refinement,
                                   (void *) &init_flag);

    // Rebuild domain if necessary
    // Will return be NULL if no refining was done?
    fclaw2d_domain_t *new_domain = fclaw2d_domain_adapt(*domain);

    if (new_domain != NULL)
    {
        // This is just for fun; remove when it gets annoying.
        // fclaw2d_domain_list_adapted(*domain, new_domain, SC_LP_STATISTICS);

        // Allocate memory for user data types (but they don't get set)
        init_domain_data(new_domain);
        copy_domain_data(*domain,new_domain);
        set_domain_time(new_domain,t);

        // Average or interpolate to new grids.
        init_block_and_patch_data(new_domain);

        // Physical BCs are needed in boundary level exchange
        // Assume only one block, since we are assuming mthbc
        int num = new_domain->num_blocks;
        for (int i = 0; i < num; i++)
        {
            fclaw2d_block_t *block = &new_domain->blocks[i];
            // This is kind of dumb for now, since block won't in general
            // have the same physical boundary conditions types.
            set_block_data(block,gparms->mthbc);
        }

        fclaw2d_domain_iterate_adapted(*domain, new_domain,cb_domain_adapt,
                                       (void *) &init_flag);

        // Set some of the user data types.  Some of this is done in
        // 'amr_set_base_level',
        // I should probably come up with a more general way to do this.
        // set_domain_data(new_domain, gparms);

        // Level stuff to make sure all
        for (int level = minlevel; level <= maxlevel; level++)
        {
            // Only do a level exchange;  Coarse and fine grid exchanges will happen
            // during time stepping as needed.
            level_exchange(new_domain,level);
            set_phys_bc(new_domain,level,t);
        }

        // free all memory associated with old domain
        amrreset(domain);
        *domain = new_domain;

        fclaw2d_domain_t *domain_partitioned =
            fclaw2d_domain_partition (*domain);

        if (domain_partitioned != NULL)
        {
            // TODO: allocate patch and block etc. memory for domain_partitioned

            // TODO: write a function to transfer values in parallel */

            /* then the old domain is no longer necessary */
            amrreset(domain);
            *domain = domain_partitioned;

            /* internal clean up */
            fclaw2d_domain_complete(*domain);
        }
    }
}
