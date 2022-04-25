KTest2(Bits);
siz = size(ans)
for i = 1:siz(1)
for j = 1:siz(2)
if ans(i,j)==1
Nums(i) =j;
end
end
end
for i = 1:siz(2)
k =1;
for j = 1:1:siz(1)
if Nums(1,j)==i
Picts1(i,k)=j;
k=k+1;
end
end
end
for i = 1:siz(2)
sum1(i) = sum(ans(:,i));
end