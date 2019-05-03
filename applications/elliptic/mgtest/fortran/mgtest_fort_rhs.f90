subroutine mgtest_fort_rhs(mbc,mx,my,xlower,ylower,dx,dy,q)
    IMPLICIT NONE

    INTEGER mbc,mx,my
    DOUBLE PRECISION xlower,ylower,dx,dy
    DOUBLE PRECISION q(1-mbc:mx+mbc,1-mbc:my+mbc)

    INTEGER rhs_choice
    COMMON /common_rhs_int/ rhs_choice

    DOUBLE PRECISION alpha,x0,y0,a,b
    COMMON /common_rhs_double/ alpha,x0,y0,a,b

    DOUBLE PRECISION pi,pi2
    COMMON /common_pi/ pi, pi2

    INTEGER i,j
    DOUBLE PRECISION x,y,r,r2, r0, hsmooth

    do i = 1-mbc,mx+mbc
        do j = 1-mbc,my+mbc
            x = xlower + (i-0.5)*dx
            y = ylower + (j-0.5)*dy
            if (rhs_choice .eq. 1) then
                r2 = (x-x0)**2 + (y-y0)**2
                q(i,j) = exp(-alpha*r2)
            elseif (rhs_choice .eq. 2) then
                q(i,j) = sin(pi*a*x)*sin(pi*b*y)
            elseif (rhs_choice .eq. 3) then
                r0 = 0.25
                r = sqrt((x-0.5)**2 + (y-0.5)**2)
                q(i,j) = Hsmooth(r + r0) - Hsmooth(r - r0)
            endif
        end do
    end do

end subroutine mgtest_fort_rhs


double precision function Hsmooth(r)
    implicit none

    double precision r

    Hsmooth = (tanh(r/0.02d0) + 1)/2.

end function Hsmooth



