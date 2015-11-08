#include "sensitivity.h"

Sensitivity::Sensitivity(int iS, int oS){
	totalQueries = 0;
	inputSize = iS; 
	outputSize = oS;
	sensitivityMatrix = new double*[inputSize];
	for (int i = 0; i < inputSize; i++){
		sensitivityMatrix[i] = new double[outputSize];
	}
	for (int i = 0; i < inputSize; i++){
		for (int j = 0; j < outputSize; j++){
			sensitivityMatrix[i][j] = 0.0;
		}
	}
}

Sensitivity::~Sensitivity(){
	for (int i = 0; i < inputSize; i++){
		delete sensitivityMatrix[i];
	}
	delete sensitivityMatrix;
}

void Sensitivity::setInputOutputMinMax(double* iMin, double* iMax, double* oMin, double* oMax){
	inputMin = iMin;
	inputMax = iMax;
	outputMin = oMin;
	outputMax = oMax;
}

void Sensitivity::pushBackInputChange(int input, double value, double delta){
	inputStack.push_back(input);
	inputDeltaStack.push_back(delta);
	inputValueStack.push_back(value);
}

void Sensitivity::pushBackOutputChange(int input, double value, double delta){
	outputStack.push_back(input);
	outputDeltaStack.push_back(delta);
	outputValueStack.push_back(value);
}

void Sensitivity::commit(){
	totalQueries++;

	//SensitivityAnalysis logic

	inputStack.clear();
	inputValueStack.clear();
	inputDeltaStack.clear();
	outputStack.clear();
	outputDeltaStack.clear();
	outputValueStack.clear();
}

void Sensitivity::generateSensitivityMatrix(){

}
