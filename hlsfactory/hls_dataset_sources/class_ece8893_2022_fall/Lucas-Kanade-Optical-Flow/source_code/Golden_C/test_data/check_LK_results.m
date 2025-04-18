%%
%clear all; close all; clc;

%%
ref_Vx = load('./ref_C_Vx.txt');
Vx = load('./out_C_Vx.txt');
diff_vx = abs(abs(ref_Vx) - abs(Vx));
fprintf('min max Vx = %4.4f %4.4f\n', min(diff_vx(:)), max(diff_vx(:)));
figure; imshow(abs(Vx), []); title 'Vx';
figure; imshow(abs(ref_Vx), []); title 'ref Vx';
figure; imshow(diff_vx, []); title 'diff Vx';
sum_diff_vx = sum(diff_vx(:))

ref_Vy = load('./ref_C_Vy.txt');
Vy = load('./out_C_Vy.txt');
diff_vy = abs(abs(ref_Vy) - abs(Vy));
fprintf('min max Vy = %4.4f %4.4f\n', min(diff_vy(:)), max(diff_vy(:)));
figure; imshow(abs(Vy), []); title 'Vy';
figure; imshow(abs(ref_Vy), []); title 'ref Vy';
figure; imshow(diff_vy, []); title 'diff Vy';
sum_diff_vy = sum(diff_vy(:))

% remove dirty values from boundaries
fprintf('tot_diff_vx = %4.4f\n', sum(sum(diff_vx(70:end-69, 70:end-69))))
fprintf('tot_diff_vy = %4.4f\n', sum(sum(diff_vy(70:end-69, 70:end-69))))

%%
    mcI1 = load('./out_C_mcI1.txt');
ref_mcI1 = load('./ref_C_mcI1.txt');
diff_mc     = abs(mcI1 - ref_mcI1);
figure; imshow(    mcI1, []); title '    mcI1';
figure; imshow(ref_mcI1, []); title 'ref mcI1';
figure; imshow(diff_mc,     []); title 'diff mcI1';
sum_diff_mc = sum(diff_mc(:))

% remove dirty values from boundaries
fprintf('tot_diff_mc = %4.4f\n', sum(sum(diff_mc(70:end-69, 70:end-69))))