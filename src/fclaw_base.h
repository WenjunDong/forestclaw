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

/** \file
 *
 * Basic include directives and declarations for ForestClaw.
 * This file helps to integrate with p4est and libsc and provides general
 * macros.  It should be included regardless of the space dimension that
 * the code is compiled for.
 */

#ifndef FCLAW_BASE_H
#define FCLAW_BASE_H

/* include config headers */

#include <fclaw_config.h>
#include <sc_config.h>
#if \
  (defined (FCLAW_ENABLE_MPI) && !defined (SC_ENABLE_MPI)) || \
  (!defined (FCLAW_ENABLE_MPI) && defined (SC_ENABLE_MPI))
#error "MPI configured differently in ForestClaw and libsc"
#endif
#if \
  (defined (FCLAW_ENABLE_MPIIO) && !defined (SC_ENABLE_MPIIO)) || \
  (!defined (FCLAW_ENABLE_MPIIO) && defined (SC_ENABLE_MPIIO))
#error "MPI I/O configured differently in ForestClaw and libsc"
#endif
#include <p4est_base.h>
#define _fclaw_const _sc_const
#define _fclaw_restrict _sc_restrict

/* start declarations */

#ifdef __cplusplus
extern "C"
{
#if 0
}                               /* need this because indent is dumb */
#endif
#endif

#ifdef __cplusplus
#if 0
{                               /* need this because indent is dumb */
#endif
}
#endif

#endif /* !FCLAW_BASE_H */
