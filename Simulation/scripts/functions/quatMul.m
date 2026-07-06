function r = quatMul(a, c)
%#codegen
    % Hamilton-Produkt:  r = a (x) c
    r = [ a(1)*c(1) - a(2)*c(2) - a(3)*c(3) - a(4)*c(4);
          a(1)*c(2) + a(2)*c(1) + a(3)*c(4) - a(4)*c(3);
          a(1)*c(3) - a(2)*c(4) + a(3)*c(1) + a(4)*c(2);
          a(1)*c(4) + a(2)*c(3) - a(3)*c(2) + a(4)*c(1) ];
end