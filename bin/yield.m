sigma=0.05;
csigma=0;
dim=2;
sig = sigma * ones( 1, dim);
var = diag( sig, 0); %=[sigma csigma; csigma sigma];

yield=yield';
[label, model, L] = vbgm(yield, 5);
[training_dim,training_size] = size(yield);
figure(1)
spread(yield, label)

[~,label(:)] = max(model.R,[],2);
index = unique(label);
count = size(index,2);
for i=1:count,
    mu(i,:) = model.m(:,index(i));
    dist_count(i)=sum(label==index(i));  %number of samples in cluster
    if(i==1)
        sigma = cat( 3, var );
    else
        sigma = cat( 3, sigma, var );
    end
end
%exact answer should (0.063, 0.758) and (0.884, 0.21)

%Guassian Mixture Distribution for Sampling
pi =  dist_count/46;
dist = gmdistribution(mu,sigma,pi);

ezcontour(@(x,y)pdf(dist,[x y]),[12.5 14],[6 7])
scatter(G_samples(:,1),G_samples(:,2),10,'.')
ezsurf(@(x,y)pdf(dist,[x y]),[12.5 14],[6 7])
