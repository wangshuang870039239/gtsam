%-----------------------------------------------------------------------
% eliminate

% the combined linear factor
Ax2 = [
-5., 0.
+0.,-5.
10., 0.
+0.,10.
];

Al1 = [
5., 0.
0., 5.
0., 0.
0., 0.
];

Ax1 = [
0.00,  0. % f4
0.00,  0. % f4
-10.,  0. % f2
0.00,-10. % f2
];

x2 = 1;
l1 = 2;
x1 = 3;

% the RHS
b2=[-1;1.5;2;-1];
sigmas = [1;1;1;1];
model4 = gtsamnoiseModelDiagonal_Sigmas(sigmas);
combined = gtsamJacobianFactor(x2, Ax2,  l1, Al1, x1, Ax1, b2, model4);

% eliminate the first variable (x2) in the combined factor, destructive !
actualCG = combined.eliminateFirst();

% create expected Conditional Gaussian
R11 = [
11.1803,  0.00
0.00, 11.1803
];
S12 = [
-2.23607, 0.00
+0.00,-2.23607
];
S13 = [
-8.94427, 0.00
+0.00,-8.94427
];
d=[2.23607;-1.56525];
expectedCG = gtsamGaussianConditional(x2,d,R11,l1,S12,x1,S13,[1;1]);
% check if the result matches
CHECK('actualCG.equals(expectedCG,1e-5)',actualCG.equals(expectedCG,1e-4));

% the expected linear factor
Bl1 = [
4.47214, 0.00
0.00, 4.47214
];

Bx1 = [
% x1
-4.47214,  0.00
+0.00, -4.47214
];

% the RHS
b1= [0.0;0.894427];

model2 = gtsamnoiseModelDiagonal_Sigmas([1;1]);
expectedLF = gtsamJacobianFactor(l1, Bl1, x1, Bx1, b1, model2);

% check if the result matches the combined (reduced) factor
% FIXME: JacobianFactor/GaussianFactor mismatch
%CHECK('combined.equals(expectedLF,1e-5)',combined.equals(expectedLF,1e-4));