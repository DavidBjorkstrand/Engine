g = 9.82;

dt = 1 / 120;
t = 0:dt:5;

p1 = zeros(2, length(t));
p1(:, 1) = [1; 0];

v1 = zeros(2, length(t));
v1(:, 1) = [0; 0];

a1 = zeros(2, length(t));
a1(:, 1) = [0; 0];

f1 = zeros(2, length(t));
f1(:, 1) = [0; 0];

p2 = zeros(2, length(t));
p2(:, 1) = [2; 0];

v2 = zeros(2, length(t));
v2(:, 1) = [0; 0];

a2 = zeros(2, length(t));
a2(:, 1) = [0; 0];

f2 = zeros(2, length(t));
f2(:, 1) = [0; 0];

p3 = zeros(2, length(t));
p3(:, 1) = [3; 0];

v3 = zeros(2, length(t));
v3(:, 1) = [0; 0];

a3 = zeros(2, length(t));
a3(:, 1) = [0; 0];

f3 = zeros(2, length(t));
f3(:, 1) = [0; 0];

p4 = zeros(2, length(t));
p4(:, 1) = [-5; 5];

v4 = zeros(2, length(t));
v4(:, 1) = [1; 0];

a4 = zeros(2, length(t));
a4(:, 1) = [0; 0];

f4 = zeros(2, length(t));
f4(:, 1) = [0; 0];

fc = zeros(8, length(t));
fc(:, 1) = [0; 0; 0; 0; 0; 0; 0; 0];

q = zeros(4, length(t));
q(:, 1) = [0; 0; 0; 0];

for i = 2:length(t)
    % Compute forces
    f1(:, i) = [0; -g];
    f2(:, i) = [0; -g];
    f3(:, i) = [0; -g];
    f4(:, i) = [0; -g];
    
    % Compute accel
    a1(:, i) = f1(:, i);
    a2(:, i) = f2(:, i);
    a3(:, i) = f3(:, i);
    a4(:, i) = f4(:, i);   
    
    % Compute position
    p1(:, i) = p1(:, i-1) + v1(:, i-1)*dt + 0.5*a1(:, i-1)*dt*dt;
    p2(:, i) = p2(:, i-1) + v2(:, i-1)*dt + 0.5*a2(:, i-1)*dt*dt;
    p3(:, i) = p3(:, i-1) + v3(:, i-1)*dt + 0.5*a3(:, i-1)*dt*dt;
    p4(:, i) = p4(:, i-1) + v4(:, i-1)*dt + 0.5*a4(:, i-1)*dt*dt;
    
    d = 1;
    k = 100000;
    a = 4/(dt*(1 + 4 * d));
    b = 4*d / (1 + 4*d);
    e = 4 / (dt*dt*k*(1 + 4 * d));

    r12 = p1(:, i) - p2(:, i);
    r23 = p2(:, i) - p3(:, i);
    r01 = p1(:, i);
    r = [0 1];
    
    if p4(2, i) < -3.85 && v4(2, i) < 0 && abs(v4(2, i)) > 0.1
        v4(2, i) = v4(2, i) - (1 + 0.99)*v4(2, i);
    elseif p4(2, i) < -3.85 && v4(2, i) < 0.0001
        G = [(r12/norm(r12))' -(r12/norm(r12))' 0 0 0 0;
             0 0 (r23/norm(r23))' -(r23/norm(r23))' 0 0;
             (r01/norm(r01))' 0 0 0 0 0 0;
             0 0 0 0 0 0 0 1];
        M = eye(8, 8);
        S = G*inv(M)*G';
        S = S + eye(size(S))*e;
        f = [f1(:, i); f2(:, i); f3(:, i); f4(:, i)];
        
        v1_pred = v1(:, i-1) + 0.5*(a1(:, i-1) + a1(:, i))*dt;
        v2_pred = v2(:, i-1) + 0.5*(a2(:, i-1) + a2(:, i))*dt;
        v3_pred = v3(:, i-1) + 0.5*(a3(:, i-1) + a3(:, i))*dt;
        v4_pred = v4(:, i-1) + 0.5*(a4(:, i-1) + a4(:, i))*dt;
        v = [v1_pred; v2_pred; v3_pred; v4_pred];
        
        q(1, i) = norm(r12) - 1;
        q(2, i) = norm(r23) - 1;
        q(3, i) = norm(r01) - 1;
        q(4, i) = p4(2, i) + 3.85;
        
        b = -dt*G*inv(M)*f - b*G*v - a*q(:, i);
        lambda = S\b;
        
        if lambda(4) < 0
            lambda(4) = 0;
        end

        fc(:, i) = (G'*lambda);
        v1(:, i) = v1(:, i-1) + 0.5*(a1(:, i-1) + a1(:, i))*dt + fc(1:2, i);
        v2(:, i) = v2(:, i-1) + 0.5*(a2(:, i-1) + a2(:, i))*dt + fc(3:4, i);
        v3(:, i) = v3(:, i-1) + 0.5*(a3(:, i-1) + a3(:, i))*dt + fc(5:6, i);
        v4(:, i) = v4(:, i-1) + 0.5*(a4(:, i-1) + a4(:, i))*dt + fc(7:8, i);
    else
        G = [(r12/norm(r12))' -(r12/norm(r12))' 0 0;
             0 0 (r23/norm(r23))' -(r23/norm(r23))'
             (r01/norm(r01))' 0 0 0 0];
        M = eye(6, 6);
        S = G*inv(M)*G';
        S = S + eye(size(S))*e;

        f = [f1(:, i); f2(:, i); f3(:, i)];

        v1_pred = v1(:, i-1) + 0.5*(a1(:, i-1) + a1(:, i))*dt;
        v2_pred = v2(:, i-1) + 0.5*(a2(:, i-1) + a2(:, i))*dt;
        v3_pred = v3(:, i-1) + 0.5*(a3(:, i-1) + a3(:, i))*dt;
        v = [v1_pred; v2_pred; v3_pred];

        q(1, i) = norm(r12) - 1;
        q(2, i) = norm(r23) - 1;
        q(3, i) = norm(r01) - 1;

%         MAX_IT = 100;
%         lambda = zeros(3, 1);
%         for it = 1:MAX_IT
%             
%             for ci = 1:3
%                 if ci == 1
%                     Bi = -dt*((r12/norm(r12))'*a1(:, i) + (-(r12/norm(r12))'*a2(:, i)));
%                     Bi = Bi - b*((r12/norm(r12))'*v1(:, i) + (-(r12/norm(r12))'*v2(:, i)));
%                     Bi = Bi - q(ci, i);
%                 elseif ci == 2
%                     Bi = -dt*((r23/norm(r23))'*a2(:, i) + (-(r23/norm(r23))'*a3(:, i)));
%                     Bi = Bi - b*((r23/norm(r23))'*v2(:, i) + (-(r23/norm(r23))'*v3(:, i)));
%                     Bi = Bi - q(ci, i);
%                 elseif ci == 3
%                     Bi = -dt*((r01/norm(r01))'*a1(:, i));
%                     Bi = Bi - b*((r01/norm(r01))'*v1(:, i));
%                     Bi = Bi - q(ci, i);
%                 end
%             
%                 
%             end
%         end
        b = -dt*G*inv(M)*f - b*G*v - a*q(1:3, i);
        lambda = S\b;

        fc(1:6, i) = (G'*lambda);

        v1(:, i) = v1(:, i-1) + 0.5*(a1(:, i-1) + a1(:, i))*dt + fc(1:2, i);
        v2(:, i) = v2(:, i-1) + 0.5*(a2(:, i-1) + a2(:, i))*dt + fc(3:4, i);
        v3(:, i) = v3(:, i-1) + 0.5*(a3(:, i-1) + a3(:, i))*dt + fc(5:6, i);
        v4(:, i) = v4(:, i-1) + 0.5*(a4(:, i-1) + a4(:, i))*dt + fc(7:8, i);
    end
end

figure;
for i = 1:length(p1)
    plot(p1(1, i), p1(2, i), 'bo');
    hold on;
    axis equal;
    plot(p2(1, i), p2(2, i), 'ro');
    plot(p3(1, i), p3(2, i), 'go');
    plot(p4(1, i), p4(2, i), 'mo');
    quiver(p1(1, i), p1(2, i), fc(1, i), fc(2, i));
    quiver(p2(1, i), p2(2, i), fc(3, i), fc(4, i)); 
    quiver(p3(1, i), p3(2, i), fc(5, i), fc(6, i)); 
    
    plot([0 p1(1, i)], [0 p1(2, i)]);
    plot([p1(1, i) p2(1, i)], [p1(2, i) p2(2, i)]);
    plot([p2(1, i) p3(1, i)], [p2(2, i) p3(2, i)]);
    plot([-6 6], [-4 -4]);
    axis([-6 6 -5 5]);
    hold off;
    pause(dt/4);
end
title('Positions');
legend('Particle 1', 'Particle 2', 'Particle 3');

figure;
plot(t, q(1:3, :));
legend('q_{12}', 'q_{23}', 'q_{01}');

figure;
plot(t, v4(2, :));
legend('Particle 4 y velocity');
