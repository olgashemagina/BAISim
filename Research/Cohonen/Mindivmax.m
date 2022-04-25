%Clusters = permute(Clusters,[1 3 2]);
Clusters(Clusters ==0)=NaN;
for i = 1:size(Clusters,3)
for j = 1:size(Clusters,3)
Min(i,j)=min(min(pdist2(Clusters(:,:,i),Clusters(:,:,j))));
end
end
Min = Min + eye(size(Min))*1e6;
Min = min(Min);
for i = 1:size(Clusters,3)
Max(1,i) = max(max(pdist(Clusters(:,:,i))));
end
K = Min./Max;