c     # ---------------------------------------------------
c     # Trace back solution to initial position     
c     # Input arguments should be the coordinates
c     # for the canonical swirl mapping in terms of 
c     # (x,y)
c     #       
c     #   T = (R*cos(2*pi*x), R*sin(2*pi*x), r*sin(2*pi*y))
c     #
c     #        r = alpha*(1 + beta*sin(2*pi*x))
c     #        R = 1 + r*cos(2*pi*y)
c     # 
c     # The solution proceeds in two steps. If we are
c     # solving an incompressible problem, we only need
c     # to evolve (x(t), y(t)).  If we have a compressible
c     # problem, we first evolve (x(t),y(t)) back to a starting
c     # location (x0,y0), and then evolve x, y and q from 
c     # the starting location back to final position.
c     # --------------------------------------------------------

      double precision function qexact(blockno, x,y,tfinal)
      implicit none

      external map_rhs_divfree, map_rhs_nondivfree
      external solout

      double precision x,y,tfinal
      integer blockno

      double precision xc0, yc0
      double precision xc1, yc1

c      integer blockno_dummy
      double precision t0
      double precision xp,yp,zp

      double precision sigma(3), rtol, atol
      integer itol, iout

      integer Nmax, lwork,nrdens, liwork
      parameter(Nmax=3, nrdens=0)
      parameter(lwork=8*Nmax+5*nrdens+21)
      parameter(liwork=nrdens+21)

      double precision work(lwork), rpar
      double precision q0_physical, q0
      integer iwork(liwork), ipar(2), idid

      double precision tol

      logical evolve_q

      integer*8 cont, get_context

      integer i

      cont = get_context()

c     # ------------------------------------------
c     # Numerical parameters
c     # ------------------------------------------
      itol = 0
      rtol = 1.d-12
      atol = 1.d-12
      iout = 0

      do i = 1,20
          work(i) = 0
          iwork(i) = 0
      enddo


c     # Evolve from t=t0 to t=tfinal
      t0 = 0

c     # Initial conditions for ODE
      sigma(1) = x
      sigma(2) = y

      ipar(1) = blockno

      call dopri5(2,map_rhs_nondivfree,t0,sigma,tfinal,
     &            rtol,atol,itol,
     &            solout,iout, work,lwork,iwork,liwork,
     &            rpar,ipar,idid)

      if (idid .ne. 1) then
          write(6,*) 'DOPRI5 : idid .ne. 1'
          stop
      endif

c     # Initial position traced back from (xc1,yc1)
      xc0 = sigma(1)
      yc0 = sigma(2)

c     # Map to [-1,1]x[-1,1]      
      call mapc2m_cart(blockno,xc0,yc0,xc1,yc1,zp)

c     # Map to [0,1]x[0,1]      
c      xp = (1 + xc0)/2.d0
c      yp = (1 + yc0)/2.d0
      xp = xc0
      yp = yc0

      q0 = q0_physical(xp,yp,zp)
      
      evolve_q = .true.
      if (evolve_q) then
c         # We now need to evolve q along with (x,y), starting from
c         # from (xc0,yc0)
          sigma(1) = xc0
          sigma(2) = yc0
          sigma(3) = q0

          do i = 1,20
              work(i) = 0
              iwork(i) = 0
          enddo

          t0 = 0
          call dopri5(3,map_rhs_nondivfree,t0,sigma,tfinal,
     &                rtol,atol,itol,
     &                solout,iout, work,lwork,iwork,liwork,
     &                rpar,ipar,idid)

          if (idid .ne. 1) then
              write(6,*) 'DOPRI5 : idid .ne. 1'
              stop
          endif

          tol = 1e-8
          if (abs(sigma(1)-x) .gt. tol) then
              write(6,*) 'qexact.f : Did not evolve x correctly'
              write(6,100) xc0,yc0
              write(6,100) x,y
              write(6,100) sigma(1), sigma(2)
              write(6,105) abs(x-sigma(1)), abs(y-sigma(2))
              stop
          endif
          if (abs(sigma(2)-y) .gt. tol) then
              write(6,*) 'qexact.f : Did not evolve y correctly'
              write(6,100) xc0,yc0
              write(6,100) x,y
              write(6,100) sigma(1), sigma(2)
              write(6,105) abs(x-sigma(1)), abs(y-sigma(2))
              stop
          endif
100       format(2F24.16)
105       format(2E24.4)          

          qexact = sigma(3)
      else
          qexact = q0
      endif

      end

c     # ---------------------------------------------------------------      
      subroutine solout(nr,xold,x,y,n,con,icomp,nd,rpar,ipar,irtrn)
      dimension y(n),con(5*nd),icomp(nd)

c     # Dummy routine

      end


c     # ----------------------------------------------------------------
c     # RHS functions for ODE solver DOPRI5  (original ode45)
c     #        -- divfree field
c     #        -- field with divergence
c     # ----------------------------------------------------------------

      subroutine map_rhs_divfree(n,t,sigma,f,rpar,ipar)
      implicit none

      external velocity_components
      integer n, ipar(2)
      double precision t, sigma(n), f(n), rpar


      integer m, i
      double precision xc1,yc1, q, u1,u2, x,y
      double precision p, px, py
      double precision t1(3), t2(3), t1xt2(3), w
      double precision u(2)
      integer blockno

      x = sigma(1)
      y = sigma(2)

      blockno = ipar(1)

      call velocity_components(blockno,x,y,u)

c     # We are tracing these back, so use negative velocities        
      f(1) = -u(1)
      f(2) = -u(2)

      end

      subroutine map_rhs_nondivfree(n,t,sigma,f,rpar,ipar)
      implicit none

      integer n, ipar(2)
      double precision t, sigma(n), f(n), rpar


      double precision x,y, q
      double precision u(2)
      double precision divu, map_divergence

      double precision t1(3), t2(3), t1xt2(3), w
      double precision p,px,py
      integer blockno

c     # Track evolution of these three quantities

      x = sigma(1)
      y = sigma(2)
      q = sigma(3)

      blockno = ipar(1)

      call velocity_components(blockno, x,y,u)

      divu = map_divergence(blockno, x,y)

      f(1) = u(1)
      f(2) = u(2)
      f(3) = -divu*q   !! Non-conservative case

      end



