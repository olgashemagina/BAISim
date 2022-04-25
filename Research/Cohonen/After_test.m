S=size(ans)
for i = 1:S(1)
for j = 1:S(2)
if ans(i,j)==1
Nums(i) =j;
end
end
end
for i = 1:S(2)
k =1;
for j = 1:S(1)
if Nums(1,j)==i
Picts1(i,k)=j;
k=k+1;
end
end
end
writematrix(Picts1, 'C:\Users\Авас yuri\Desktop\Picts2.csv')