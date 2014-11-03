
function [xp,yp,zp] = mapc2m(xc1,yc1)

global notpillowsphere;

% map = 'pillowsphere';
map = 'cubedsphere';

switch map
    case 'pillowsphere'
        notpillowsphere = false;
        [xp,yp,zp] = mapc2m_pillowsphere(xc1,yc1);
    case 'cubedsphere'
        [xp,yp,zp] = mapc2m_cubedsphere(xc1,yc1);
        b = getblocknumber();
        s = 0.1;
        switch b
            case 0
                zp = zp - s;
            case 1
                yp = yp + s;
            case 2
                xp = xp - s;
            case 3
                zp = zp + s;
            case 4
                yp = yp - s;
            case 5
                xp = xp + s;
            otherwise
        end
        %{
            case 2
                xp = xp + s;
            case 3
                yp = yp - s;
            case 4
                yp = yp + s;
            case 5
                zp = zp + s;
        end
        %}
end


end