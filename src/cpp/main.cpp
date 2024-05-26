//
// Created by Talha Jamal on 19/05/2024.
//
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <sstream>
#include "csv.h"
#include "read_data.h"
#include "linearAlgebra.h"
#include "portfolio.h"

using namespace std;

int  main (int  argc, char  *argv[])
{
    int numberAssets = 83; // Initialize Number of Assets
    int numberReturns = 700; // Max Length of Returns Data
    // Set list of increasing target Portfolio Returns
    const int steps = 20;
    double temp[steps];
    double start = 0.005;
    double end = 0.1;
    double increment = (end - start) / (steps - 1);
    // Backtesting Parameters
    double epsilon = 1e-7;
    int isWindow = 100;
    int oosWindow = 12;
    int slidingWindow = 12;
    int numOfSlidingWindows = (numberReturns - isWindow - oosWindow) / (slidingWindow);

    vector<Portfolios> TargetReturnsPortfolios;
    string desiredDirectory = "/Users/talhajamal/Desktop/Code/Portfolio_Optimizer_CPP"; // Home Directory
    changeWorkingDirectory(desiredDirectory); // Change to Correct Working Directory
    string fileName = "data/asset_returns.csv"; // Return Data FileName
    checkFileInCurrentDirectory(fileName); // Check if File Exists and File Path is correct

    Matrix returnMatrix;
    returnMatrix.resize(numberReturns);
    for(int i=0;i<numberReturns;i++)
        returnMatrix[i].resize(numberAssets);
    readData(returnMatrix,fileName); // Read return data from the file and store in 2D returnMatrix

    for (int i = 0; i < steps; ++i) {temp[i] = start + i * increment;}
    Vector tReturns(temp, temp + steps);

    ofstream resultsFile; // Create CSV File to send results to
    resultsFile.open("data/results.csv");
    resultsFile << "Target Returns,";
    for (int i = 0; i < numOfSlidingWindows - 1; ++i)
    {
        resultsFile << "Backtest Period: " << i+1 << ",";
    }
    resultsFile << "Backtest Period: " << numOfSlidingWindows << endl;

    for (int i = 0; i < 20; i++)
    {
        cout << "=================================================================" << endl;
        cout << "Running a Backtest for Portfolio with Target Returns of : " << tReturns[i] << endl;
        Portfolios portfolio(isWindow, oosWindow, slidingWindow, numOfSlidingWindows, returnMatrix, tReturns[i]);
        portfolio.calculateIsMean();
        //printMatrix(portfolio.getISMean(), "Mean Return in all different periods: ");
        portfolio.calculateIsCovMat();
        //printMatrix(portfolio.getISCovMat()[0], "IS CovMatrix in Period 0");
        //cout << portfolio.getISCovMat()[0][0].size() << endl;
        portfolio.calculateOOSMean();
        //printMatrix(portfolio.getOSMean(), "OS Mean Return");
        //cout << portfolio.getOSMean().size() << endl;
        portfolio.calculateOOSCovMatrix();
        //printMatrix(portfolio.getOSCovMat()[0], "OOS Cov Matrix in Period 0");
        portfolio.calculateQ();
        //printMatrix(portfolio.getQ()[0], "Matrix Q in Period 0");
        portfolio.optimizer(epsilon);
        //printMatrix(portfolio.getWeights(), "Weights");
        portfolio.runBacktest();
        TargetReturnsPortfolios.push_back(portfolio);

        //cout << "With a Target Return of " << tReturns[i]*100 << " the actual return of the Portfolio is: " << endl;
        Vector actualAvgReturn = portfolio.getActualAverageReturn();
        //printVector(actualAvgReturn, "Actual Average Returns");
        cout << "Portfolio's Actual Average Abnormal Return is: " << portfolio.getAvgAbnormalReturn() << endl;
        cout << "Portfolio's Sharpe Ratio is: " << portfolio.getPortfolioSharpeRatio() << endl;
        cout << "Portfolio's Cumulate Average Return is: " << portfolio.getCumulativeAvgAbnormalReturn() << endl;

        resultsFile << tReturns[i]*100 << ",";
        for (int j = 0; j < actualAvgReturn.size()-1; j++){
            resultsFile << actualAvgReturn[j] << ",";
        }
        resultsFile << actualAvgReturn[actualAvgReturn.size()-1] << endl;
    }
    return 0;
}
