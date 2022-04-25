res = load( 'res.csv');% Load .csv from Img2FeatureVector

resonly = res(:,3:size(res,2)); % res without 2 first columns (Nvideo, Nframe)

nnstart % GUI for Deep Learning Toolbox; ->SOM, Learn,Deploy in matrix-only function;

A3_16(resonly);%function of SOM

AllTo Clusters - csv (rows - NCluster, columns - globalNframe) and Measure.