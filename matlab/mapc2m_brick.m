function [xp,yp,zp] = mapc2m_brick(xc,yc)

% (mi,mj) brick

brick_data = load('brick.dat');
mi = brick_data(1,1);
mj = brick_data(1,2);
xv = brick_data(2:end,1);
yv = brick_data(2:end,2);

blockno = getblocknumber();

xp = (xv(blockno+1) + xc)/mi;
yp = (yv(blockno+1) + yc)/mj;
zp = 0*xp;

end