% res = load( 'C:\Users\Авас yuri\Desktop\3\res.csv');
% resonly = res(:,3:size(res,2));
% A3_16(resonly);%function of SOM
After_test;
S=size(ans);
for i = 1:S(2)
k = 1;
for j = 1:S(1)
if ans(j,i)==1
Clusters(k,i,:)=resonly(j,:);
k=k+1;
end
end
end
Clusters = permute(Clusters,[1 3 2]);
Mindivmax;