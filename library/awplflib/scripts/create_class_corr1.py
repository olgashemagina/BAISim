#!/usr/bin/env python
# coding: utf-8

import numpy as np
import scipy
from sklearn.decomposition import PCA
from sklearn.cluster import KMeans
import xml.etree.ElementTree as ET

import struct

import os

os.environ['OMP_NUM_THREADS'] = '1'

class BaselineCorrector:
    def __init__(self):
        CRLS: np.ndarray = None
        WRLS: np.ndarray = None
        numClust: int = None
        centre: np.ndarray = None
        project: np.ndarray = None
        centroids: np.ndarray = None
        FD: np.ndarray = None
        treshs: np.ndarray = None
        self.train_logs: np.ndarray = []


    def fit(self, CRLS, WRLS, space='reduced', whitening=False, setPC=3, numPC=150, numClust=15, nBins = 100):
        # Инициализация
        
        self.CRLS = CRLS
        self.WRLS = WRLS
        self.numClust = numClust
        numPC = min(CRLS.shape[1], numPC)
        # Расчет центра в зависимости от setPC
        match setPC:
            case 1:
                all = np.concatenate((CRLS, WRLS), axis=0)
                centre = np.mean(all, axis=0)
            case 2:
                centre = np.mean(CRLS, axis=0)
            case 3:
                centre = np.mean(WRLS, axis=0)
            case _:
                raise ValueError("Invalid value for setPC. Valid options are: 1 (CRLS+WRLS), 2 (CRLS), or 3 (WRLS).")

        self.centre = centre
        self.space = space

        # Проверка на корректность значения space
        if space == "original":
            # Центрирование данных
            CRLS = CRLS - self.centre
            WRLS = WRLS - self.centre
            
            if whitening:
                match setPC:
                    case 1:
                        both = np.concatenate((CRLS, WRLS), axis=0)
                        project = np.std(both, axis=0)
                    case 2:
                        project = np.std(CRLS, axis=0)
                    case 3:
                        project = np.std(WRLS, axis=0)
            else:
                project = np.ones(self.centre.shape)

            self.project = project

            # Нормализация данных
            CRLSR = np.divide(CRLS, project, where=project!=0, out=np.zeros_like(CRLS))
            WRLSR = np.divide(WRLS, project, where=project!=0, out=np.zeros_like(WRLS))

        elif space == "reduced":
            # PCA для уменьшения размерности
            pca = PCA(n_components=numPC, whiten=whitening)
            match setPC:
                case 1:
                    both = np.concatenate((CRLS, WRLS), axis=0)
                    pca.fit(both)
                case 2:
                    pca.fit(CRLS)
                case 3:
                    pca.fit(WRLS)
            CRLSR = pca.transform(CRLS)
            WRLSR = pca.transform(WRLS)
            self.project = pca.components_.T
        else:
            raise ValueError("Invalid value for space. Valid options are: 'original' or 'reduced'.")

        # Кластеризация
        kmeans = KMeans(n_clusters=numClust, random_state=0, n_init="auto").fit(WRLSR)
        self.centroids = kmeans.cluster_centers_

        labels = kmeans.labels_
        for i in range(numClust):
            if not (labels == i).sum():
                labels = [x-1 if x > i else x for x in labels]
                self.centroids = np.delete(self.centroids, i, axis=0)

        self.numClust = self.centroids.shape[0]

        # Расчет дискриминантных векторов
        covCRLSR = np.cov(CRLSR.T)
        meanCRLSR = np.mean(CRLSR, axis=0)
        self.FD = np.zeros((numPC, self.numClust))
        for k in range(self.numClust):
            tmp = WRLSR[np.where(labels == k)]
            if tmp.shape[0] > 1:
                cov1 = np.cov(tmp.T)
            else:
                cov1 = np.zeros((WRLSR.shape[1], WRLSR.shape[1]))
            self.FD[:, k] = np.linalg.solve(covCRLSR + cov1, meanCRLSR - np.mean(tmp, axis=0))

        self.FD /= np.sqrt(np.sum(self.FD ** 2, axis=0))

        projection_crls_train = self.splitToClusters(
            data = (self.CRLS - self.centre) @ self.project,
            centroids = self.centroids,
            FD = self.FD
        )

        projection_wrls_train = self.splitToClusters(
            data = (self.WRLS - self.centre) @ self.project,
            centroids = self.centroids,
            FD = self.FD
        )

        self.train_logs = np.zeros((self.numClust, 6))
        self.treshs = np.zeros(self.numClust)

        for k in range(self.numClust):
            self.train_logs[k] = self.oneClusterGraph(projection_crls_train[k], projection_wrls_train[k], round(nBins / 10.),
                    "Distribution of training set for cluster %03d" % k, "far")
            self.treshs[k] = self.train_logs[k, 2]

        return None

    def correct(self, X):
        projection_test = self.splitToClusters(
            data = (X - self.centre) @ self.project,
            centroids = self.centroids,
            FD = self.FD
        )

        # print((X - self.centre) @ self.project)
        
        error_prediction = []
        count = 0
       
        for cluster_number in range(self.numClust):
            for elem in projection_test[cluster_number]:
                if elem > self.treshs[cluster_number]:
                    error_prediction.append('CR')
                else:
                    error_prediction.append('WR')
            
        for i in error_prediction:
            if i == "WR":
                count = count + 1
        return count

    
    @staticmethod
    def splitToClusters(data, centroids, FD):
    #% Inputs:
    #%   data is set of data points, one point in row.
    #%   centroids matrix of centroids for clusters with one centroid per row
    #%   FD is matrix of Fisher's discriminant directions. One direction per
    #%       column.
    #% Outputs:
    #%   res is cell array. Each cell contains projections of datapoints of one
    #%       cluster onto corresponding FD of this cluster.

    #    %  Create array for output
        #print(centroids.shape, data.shape)
        nClust = FD.shape[1]
        res = []
    #    % Calculate distances to centroids
        dist = scipy.spatial.distance.cdist(data, centroids)
        #print(dist.shape); raise
    #    dist = np.sum(data ** 2, axis=1) + np.sum(centroids.conj().T ** 2, axis=0)
    #    dist -= 2 * data @ centroids.conj().T
    #    % Find the minimal distance
        lab = np.argmin(dist, axis=1)
    #    % Identify all points with the closest centroids and calculate
    #    % projections on FD
        for k in range(nClust):
            proj = data[lab == k] @ FD[:, k]
            res.append(proj)
        return res
    
    
    @staticmethod
    def oneClusterGraph(x, y, nBins, name, thresOptim, prThres=None):
    #%oneDClass applied classification with one input attribute by searching
    #%the best threshold.
    #%
    #% Inputs:
    #%   x contains values for the first class
    #%   y contains values for the second class
    #%   nBins is number of bins to use
    #%   name is string with title of axis
    #%   prThres is predefined threshold.
    #%   thresOptim - parameter responsible for threshold determination. In case of "basic" threshold'll be 
    #%            optimal to minimize half of sencsitivity + specificity or 0.5*(TP/Pos+TN/Neg).
    #%            In case of "far" threshold'll be set to make FPR = 0
    #%            In case of "frr" threshold'll be set to make FNR = 0
    #%   
    #% Outputs:
    #%   res is row vector with 6 values (9 if predefined threshold is
    #%       specified): 
    #%       res(1) is number of cases of the first class
    #%       res(2) is number of cases of the second class
    #%       res(3) is the best threshold for balanced accuracy:
    #%           half of sencsitivity + specificity or 
    #%               0.5*(TP/Pos+TN/Neg), where 
    #%           TP is True positive or the number of correctly recognised
    #%               casses of the first class (, 
    #%           Pos is the number of casses of the first class (res(1)),
    #%           TN is True negative or the number of correctly recognised
    #%               casses of the secong class, 
    #%           Neg is the number of casses of the second class.
    #%       res(4) is True positive rate or TP/Pos
    #%       res(5) is True negative rate or TN/Neg
    #%       res(6) is Balanced accuracy
    #%       res(7) is True positive rate or TP/Pos for predefined threshold
    #%       res(8) is True negative rate or TN/Neg for predefined threshold
    #%       res(9) is Balanced accuracy for predefined threshold
    #%

    #    % Create Array for result
        if prThres is not None:
            res = np.zeros(9)
        else:
            res = np.zeros(6)

    #    % Define numbers of cases
        Pos = len(x)
        Neg = len(y)
        res[0] = Pos
        res[1] = Neg

    #    % do we have both classes?
        if Pos == 0 or Neg == 0:
            if Pos + Neg == 0:
                return res

            if Pos > 0:
                res[2] = np.min(x)
                res[2] = res[2] - 0.001 * abs(res[2])
                res[3] = 1
                if prThres is not None:
                    res[6] = np.sum(np.where(x > prThres)) / Pos

            else:
                res[2] = np.max(y)
                res[2] = res[2] + 0.001 * abs(res[2])
                res[4] = 1
                if prThres is not None:
                    res[7] = np.sum(np.where(y > prThres)) / Neg

            res[5] = 1
            return res
            
    #    % Define set of unique values

        thr = np.unique(np.concatenate((x, y))).conj().T
    #    % Add two boders
        #thr = [thr[0] - 0.0001 * abs(thr[0]), (thr[1:] + thr[0:-1]) / 2, thr[-1] + 0.0001 * abs(thr[-1])]
        tmp = ((thr[1:] + thr[0:-1]) / 2)
        np.append(tmp, thr[-1] + 0.0001 * abs(thr[-1]))
        np.insert(tmp, 0, thr[0] - 0.0001 * abs(thr[0]))
        thr=tmp
        accs = np.zeros(len(thr))
        
    #    % Define meaning of "class 1"
        xLt = np.mean(x) > np.mean(y)
        print("xLt = ", xLt)
        print("X: ", x)
        print("Y: ", y)
    #    % Define variabled to search
        bestAcc = 0
        bestT = -np.inf
        bestTPR = 0
        bestTNR = 0
    #    %Check each threshold
        for k in range(len(thr)):
            t = thr[k]
            nX = np.sum(x < t)
            nY = np.sum(y >= t)
            if xLt:
                nX = Pos - nX
                nY = Neg - nY
            if(thresOptim == "far"):
                far = (Pos-nX) / Pos
                if(far != 0):
                    if(k<=1):
                        bestT = -100000
                    else:
                        bestT = thr[k-2]
                    break
                else:
                    bestT = thr[k]
            acc = (nX / Pos + nY / Neg) / 2
            if acc > bestAcc:
                bestAcc = acc
                bestT = t
                bestTPR = nX / Pos
                bestTNR = nY / Neg


            accs[k] = acc
        print(bestT, far)
        res[2] = bestT
        res[3] = bestTPR
        res[4] = bestTNR
        res[5] = bestAcc

        if prThres is not None:
            nX = np.sum(np.where(x < prThres))
            nY = np.sum(np.where(y >= prThres))
            if xLt:
                nX = Pos - nX
                nY = Neg - nY

            res[6] = nX / Pos
            res[7] = nY / Neg
            res[8] = (nX / Pos + nY / Neg) / 2
            
        return res
    
    
    def save_state(self, path):
        root = ET.Element("BaselineCorrector")

        ET.SubElement(root, "numClust").text = str(self.numClust)

        centre_elem = ET.SubElement(root, "centre")
        centre_elem.text = " ".join(map(str, self.centre))

        project_elem = ET.SubElement(root, "project")
        for row in self.project:
            row_elem = ET.SubElement(project_elem, "row")
            row_elem.text = " ".join(map(str, row))

        centroids_elem = ET.SubElement(root, "centroids")
        for centroid in self.centroids:
            centroid_elem = ET.SubElement(centroids_elem, "centroid")
            centroid_elem.text = " ".join(map(str, centroid))

        FD_elem = ET.SubElement(root, "FD")
        for row in self.FD:
            row_elem = ET.SubElement(FD_elem, "row")
            row_elem.text = " ".join(map(str, row))

        treshs_elem = ET.SubElement(root, "treshs")
        treshs_elem.text = " ".join(map(str, self.treshs))

        tree = ET.ElementTree(root)
        with open(path, "wb") as f:
            tree.write(f, encoding="utf-8", xml_declaration=True)