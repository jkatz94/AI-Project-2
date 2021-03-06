/*
*****************************************
Jason Katz
ECE-469 Project 2: Neural Network

Neural network implementation
with one hidden layer

File: Network.cpp
Description: Neural network class
definitions and algorithms
*****************************************
*/

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <math.h>
#include "Network.h"

using namespace std;

Network::Network(vector<Node *> i, vector<Node *> h, vector<Node *> o) {
	inputLayer = i;
	hiddenLayer = h;
	outputLayer = o;
}

void Network::Train(string fileName, int epochs, double learningRate) {
	/*
	 * Load training data
	 */

	string line;
	stringstream ss;

	// Get training data parameters
	ifstream trainFile(fileName);
	int numExamples, numInputs, numOutputs;
	getline(trainFile, line);
	ss.str(line);
	ss >> numExamples >> numInputs >> numOutputs;
	ss.clear();

	// Load examples here so we don't have to do it every epoch
	vector<vector<double>> inputExamples, outputExamples;
	inputExamples.resize(numExamples);
	outputExamples.resize(numExamples);
	int index = 0;
	while (getline(trainFile, line)) {
		ss.str(line);
		inputExamples[index].resize(numInputs);
		outputExamples[index].resize(numOutputs);
		for (int i = 0; i < numInputs; ++i) {
			ss >> inputExamples[index][i];
		}
		for (int i = 0; i < numOutputs; ++i) {
			ss >> outputExamples[index][i];
		}
		++index;
		ss.clear();
	}
	trainFile.close();

	// Main loop
	for (int num = 0; num < epochs; ++num) {

		// Cycle through all examples
		for (int ex = 0; ex < numExamples; ++ex) {

			// Set all activations in input layer equal to the example (skip fixed input node)
			for (int i = 1; i < numInputs + 1; ++i) {
				inputLayer[i]->activation = inputExamples[ex][i - 1];
			}

			// Propagate inputs forward to hidden layer (skip fixed hidden node)
			for (unsigned int i = 1; i < hiddenLayer.size(); ++i) {
				// Compute new in for this node
				hiddenLayer[i]->in = 0;
				for (unsigned int j = 0; j < hiddenLayer[i]->weights.size(); ++j) {
					hiddenLayer[i]->in += (hiddenLayer[i]->weights[j] * inputLayer[j]->activation);
				}
				// Set new activation
				hiddenLayer[i]->activation = sigmoid(hiddenLayer[i]->in);
			}

			// Propagate hidden nodes forward to output layer
			for (unsigned int i = 0; i < outputLayer.size(); ++i) {
				// Compute new in for this node
				outputLayer[i]->in = 0;
				for (unsigned int j = 0; j < outputLayer[i]->weights.size(); ++j) {
					outputLayer[i]->in += (outputLayer[i]->weights[j] * hiddenLayer[j]->activation);
				}
				// Set new activation
				outputLayer[i]->activation = sigmoid(outputLayer[i]->in);
			}

			// Back propagate errors from output layer to input layer

			// Calculate errors at the output layer
			for (unsigned int i = 0; i < outputLayer.size(); ++i) {
				outputLayer[i]->error = sigmoidPrime(outputLayer[i]->in) * (outputExamples[ex][i] - outputLayer[i]->activation);
			}

			// Calculate errors at the hidden layer (skip fixed node)
			for (unsigned int i = 1; i < hiddenLayer.size(); ++i) {
				hiddenLayer[i]->error = 0;
				for (unsigned int j = 0; j < outputLayer.size(); ++j) {
					hiddenLayer[i]->error += (outputLayer[j]->weights[i] * outputLayer[j]->error);
				}
				hiddenLayer[i]->error *= sigmoidPrime(hiddenLayer[i]->in);
			}

			// Update output node weights
			for (unsigned int i = 0; i < outputLayer.size(); ++i) {
				for (unsigned int j = 0; j < outputLayer[i]->weights.size(); ++j) {
					outputLayer[i]->weights[j] += (learningRate * hiddenLayer[j]->activation * outputLayer[i]->error);
				}
			}

			// Update hidden node weights
			for (unsigned int i = 1; i < hiddenLayer.size(); ++i) {
				for (unsigned int j = 0; j < hiddenLayer[i]->weights.size(); ++j) {
					hiddenLayer[i]->weights[j] += (learningRate * inputLayer[j]->activation * hiddenLayer[i]->error);
				}
			}

		}

	}
}

void Network::Test(string testFileName, string resultsFileName) {
	/*
	* Load testing data
	*/

	string line;
	stringstream ss;

	// Get training data parameters
	ifstream testFile(testFileName);
	int numExamples, numInputs, numOutputs;
	getline(testFile, line);
	ss.str(line);
	ss >> numExamples >> numInputs >> numOutputs;
	ss.clear();

	// Load testing examples here
	vector<vector<double>> inputExamples, outputExamples;
	inputExamples.resize(numExamples);
	outputExamples.resize(numExamples);
	int index = 0;
	while (getline(testFile, line)) {
		ss.str(line);
		inputExamples[index].resize(numInputs);
		outputExamples[index].resize(numOutputs);
		for (int i = 0; i < numInputs; ++i) {
			ss >> inputExamples[index][i];
		}
		for (int i = 0; i < numOutputs; ++i) {
			ss >> outputExamples[index][i];
		}
		++index;
		ss.clear();
	}
	testFile.close();

	// Get test results:
	// 0 for predicted == 1, actual == 1
	// 1 for predicted == 1, actual == 0
	// 2 for predicted == 0, actual == 1
	// 3 for predicted == 0, actual == 0
	vector<vector<int>> results;
	results.resize(numExamples);

	// Set all activations in input layer equal to the example (skip fixed input node)
	for (int ex = 0; ex < numExamples; ++ex) {
		results[ex].resize(numOutputs);

		for (int i = 1; i < numInputs + 1; ++i) {
			inputLayer[i]->activation = inputExamples[ex][i - 1];
		}

		// Propagate inputs forward to hidden layer (skip fixed hidden node)
		for (unsigned int i = 1; i < hiddenLayer.size(); ++i) {
			// Compute new in for this node
			hiddenLayer[i]->in = 0;
			for (unsigned int j = 0; j < hiddenLayer[i]->weights.size(); ++j) {
				hiddenLayer[i]->in += (hiddenLayer[i]->weights[j] * inputLayer[j]->activation);
			}
			// Set new activation
			hiddenLayer[i]->activation = sigmoid(hiddenLayer[i]->in);
		}

		// Propagate hidden nodes forward to output layer
		for (unsigned int i = 0; i < outputLayer.size(); ++i) {
			// Compute new in for this node
			outputLayer[i]->in = 0;
			for (unsigned int j = 0; j < outputLayer[i]->weights.size(); ++j) {
				outputLayer[i]->in += (outputLayer[i]->weights[j] * hiddenLayer[j]->activation);
			}
			// Set new activation
			outputLayer[i]->activation = sigmoid(outputLayer[i]->in);
		}

		for (unsigned int i = 0; i < outputLayer.size(); ++i) {
			if (outputLayer[i]->activation > .5 && outputExamples[ex][i]) {
				results[ex][i] = 0;
			} else if (outputLayer[i]->activation > .5 && !outputExamples[ex][i]) {
				results[ex][i] = 1;
			} else if (outputLayer[i]->activation < .5 && outputExamples[ex][i]) {
				results[ex][i] = 2;
			} else if (outputLayer[i]->activation < .5 && !outputExamples[ex][i]) {
				results[ex][i] = 3;
			}
		}
	}

	// Compute metrics
	double overallAccuracy, precision, recall, f1;
	double totalA = 0, totalB = 0, totalC = 0, totalD = 0;
	double totalOverallAccuracy = 0, totalPrecision = 0, totalRecall = 0, totalF1 = 0;
	vector<double> As, Bs, Cs, Ds;
	vector<double> overallAccuracies, precisions, recalls, f1s;
	for (int i = 0; i < numOutputs; ++i) {
		double A = 0, B = 0, C = 0, D = 0;
		for (int j = 0; j < numExamples; ++j) {
			if (results[j][i] == 0) {
				++A;
			} else if (results[j][i] == 1) {
				++B;
			} else if (results[j][i] == 2) {
				++C;
			} else if (results[j][i] = 3) {
				++D;
			}
		}
		overallAccuracy = (A + D) / (A + B + C + D);
		precision = A / (A + B);
		recall = A / (A + C);
		f1 = 2 * precision * recall / (precision + recall);

		totalOverallAccuracy += overallAccuracy;
		totalPrecision += precision;
		totalRecall += recall;
		totalF1 += f1;
		overallAccuracies.push_back(overallAccuracy);
		precisions.push_back(precision);
		recalls.push_back(recall);
		f1s.push_back(f1);
		As.push_back(A);
		Bs.push_back(B);
		Cs.push_back(C);
		Ds.push_back(D);
		totalA += A;
		totalB += B;
		totalC += C;
		totalD += D;
	}

	double microOverallAccuracy = (totalA + totalD) / (totalA + totalB + totalC + totalD);
	double microPrecision = totalA / (totalA + totalB);
	double microRecall = totalA / (totalA + totalC);
	double microF1 = 2 * microPrecision * microRecall / (microPrecision + microRecall);
	double macroOverallAccuracy = totalOverallAccuracy / numOutputs;
	double macroPrecision = totalPrecision / numOutputs;
	double macroRecall = totalRecall / numOutputs;
	double macroF1 = 2 * macroPrecision * macroRecall / (macroPrecision + macroRecall);

	// Write results to file
	ofstream resultsFile(resultsFileName);
	for (int i = 0; i < numOutputs; ++i) {
		resultsFile << (int) As[i] << " " << (int) Bs[i] << " " << (int) Cs[i] << " " << (int) Ds[i] << " ";
		resultsFile.precision(3);
		resultsFile << fixed << overallAccuracies[i] << " " << precisions[i] << " " << recalls[i] << " " << f1s[i] << endl;
	}
	resultsFile << microOverallAccuracy << " " << microPrecision << " " << microRecall << " " << microF1 << endl;
	resultsFile << macroOverallAccuracy << " " << macroPrecision << " " << macroRecall << " " << macroF1 << endl;
}

Network Network::LoadFromFile(string fileName) {
	/*
	 * Load initial data from file
	 */

	ifstream initFile(fileName);

	// Get number of nodes in each layer
	int numInputs, numHidden, numOutputs;
	initFile >> numInputs >> numHidden >> numOutputs;

	// Initialize all layers
	vector<Node *> inputs, hiddens, outputs;
	inputs.resize(numInputs + 1);
	hiddens.resize(numHidden + 1);
	outputs.resize(numOutputs);

	// Initialize input nodes
	Node * inputFixed = new Node();
	inputFixed->activation = -1;
	inputs[0] = inputFixed;
	for (int i = 1; i < numInputs + 1; ++i) {
		Node * n = new Node();
		inputs[i] = n;
	}

	// Initialize hidden nodes
	Node * hiddenFixed = new Node();
	hiddenFixed->activation = -1;
	hiddens[0] = hiddenFixed;
	for (int i = 1; i < numHidden + 1; ++i) {
		Node * n = new Node();
		vector<double> weights;
		for (int j = 0; j < numInputs + 1; ++j) {
			double w;
			initFile >> w;
			weights.push_back(w);
		}
		n->weights = weights;
		hiddens[i] = n;
	}

	// Initialize output nodes
	for (int i = 0; i < numOutputs; ++i) {
		Node * n = new Node();
		vector<double> weights;
		for (int j = 0; j < numHidden + 1; ++j) {
			double w;
			initFile >> w;
			weights.push_back(w);
		}
		n->weights = weights;
		outputs[i] = n;
	}

	initFile.close();

	return Network(inputs, hiddens, outputs);
}

void Network::SaveToFile(string fileName, Network n) {
	ofstream saveFile(fileName);

	// Write numbers of nodes to first line
	saveFile << n.inputLayer.size() - 1 << " " << n.hiddenLayer.size() - 1 << " " << n.outputLayer.size() << endl;

	// Set double precision
	saveFile.precision(3);
	saveFile << fixed;
	
	// Write weights of hidden layers to next lines (skip fixed node)
	for (unsigned int i = 1; i < n.hiddenLayer.size(); ++i) {
		for (unsigned int j = 0; j < n.hiddenLayer[i]->weights.size(); ++j) {
			saveFile << n.hiddenLayer[i]->weights[j];
			if (j != n.hiddenLayer[i]->weights.size() - 1) {
				saveFile << " ";
			}
		}
		saveFile << endl;
	}

	// Write weights of output layers to next lines
	for (unsigned int i = 0; i < n.outputLayer.size(); ++i) {
		for (unsigned int j = 0; j < n.outputLayer[i]->weights.size(); ++j) {
			saveFile << n.outputLayer[i]->weights[j];
			if (j != n.outputLayer[i]->weights.size() - 1) {
				saveFile << " ";
			}
		}
		saveFile << endl;
	}

	saveFile.close();
}

double Network::sigmoid(double num) {
	return 1.0 / (1.0 + exp(-num));
}

double Network::sigmoidPrime(double num) {
	return sigmoid(num) * (1.0 - sigmoid(num));
}