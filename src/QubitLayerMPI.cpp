#include "QubitLayerMPI.h"

using namespace std;

template <typename... T>
void QubitLayerMPI::appendResultLog(const T&... args)
{
	((this->resultLog << args), ...);
}

template <typename... T>
void QubitLayerMPI::appendDebugLog(const T&... args)
{
	((this->debugLog << args), ...);
}

void QubitLayerMPI::outputDebugLogs(bool to_stdout)
{
	// remove all previous logs
	if(rank == 1) {
		filesystem::remove_all("logs");
		filesystem::create_directory("logs");
	}
	MPI_Barrier(MPI_COMM_WORLD);

	// create global log file
	string globalLogFileName = "logs/globalLog.txt";
	fstream globalLogFile;

	// create log file with name according to rank
	string nodeFileName = "logs/log";
	nodeFileName.append(to_string(rank));
	nodeFileName.append(".txt");
	fstream nodeLogFile;

	// Write to each log file
	nodeLogFile.open(nodeFileName, ios::app);

	if(!nodeLogFile) {
		cout << "file does not exist" << endl;
	} else {
		// Print logs orderly
		for(int i = 0; i < size; i++) {
			if(rank == i)
				nodeLogFile << this->debugLog.str() << "\n";
			else
				MPI_Barrier(MPI_COMM_WORLD);
		}
		nodeLogFile.close();
	}

	// Write to global log file
	globalLogFile.open(globalLogFileName, ios::app);

	if(!globalLogFile) {
		cout << "file does not exist" << endl;
	} else {
		globalLogFile << this->debugLog.str() << "\n";
		globalLogFile.close();
	}

	if(to_stdout) {
		// Print logs orderly
		for(int i = 0; i < size; i++) {
			if(rank == i)
				cout << this->debugLog.str() << endl;
			else
				MPI_Barrier(MPI_COMM_WORLD);
		}
	}
}

bool QubitLayerMPI::checkStateOOB(bitset<numQubitsMPI> state)
{
	size_t lowerBound = this->rank * (this->states.size() / 2);
	size_t upperBound = (this->rank + 1) * (this->states.size() / 2);

	return state.to_ulong() < lowerBound || state.to_ulong() >= upperBound;
}

vector<complex<double>>
QubitLayerMPI::handlerStatesOOB(vector<complex<double>> statesOOB)
{
	// Receives the vector statesOOB that contains (state, intended_value) pairs,
	// Searches for each state the node that owns the state,
	// Sends a message for the respective node, the state and value for the operation,
	// Sends -1 for every node that had no intended message
	// Receives every message, applies the operation of a received message if its not -1

	// Create vector with all ranks to keep track ranks that have no intended operations
	vector<int> ranks;
	for(int i = 0; i < size; i++) {
		ranks.push_back(i);
	}

	// send messages for all states in the vector
	int node = -1, nextNode = -1;
	vector<complex<double>> msgToSend;

#ifdef HANDLER_STATES_DEBUG
	if(statesOOB.size() != 0) {
		debugLog.append("Process ");
		debugLog.append(to_string(rank));
		debugLog.append(" wants to send ");
		debugLog.append("this: \n");
		for(size_t i = 0; i < statesOOB.size(); i += 2) {
			debugLog.append("\t");
			debugLog.append(to_string(statesOOB[i].real()));
			debugLog.append(" ");
			debugLog.append(to_string(statesOOB[i + 1].real()));
			debugLog.append("\n");
		}
	}
#endif

	for(size_t i = 0; i < statesOOB.size(); i += 2) {
		node = getNodeOfState(statesOOB[i].real());

		msgToSend.push_back(statesOOB[i]);
		msgToSend.push_back(statesOOB[i + 1]);

		if(!(i + 2 > statesOOB.size())) {
			nextNode = getNodeOfState(statesOOB[i + 2].real());

			if(nextNode != node || i + 2 == statesOOB.size()) {
				// termina o buffer msgToSend, envia e faz clear

#ifdef HANDLER_STATES_DEBUG
				cout << "Process " << rank << " sending to node " << node << endl;
				debugLog.append("Process ");
				debugLog.append(to_string(rank));
				debugLog.append(" sending to node ");
				debugLog.append(to_string(node));
				debugLog.append("\n");
				for(size_t z = 0; z < msgToSend.size(); z += 2) {
					debugLog.append("\t");
					debugLog.append(to_string(msgToSend[z].real()));
					debugLog.append(" ");
					debugLog.append(to_string(msgToSend[z + 1].real()));
					debugLog.append("\n");
				}
				debugLog.append("\n");
#endif

				// Erase the rank that has a intended operation
				ranks.erase(ranks.begin() + node);

				// Convert vector into an array acceptable for MPI
				complex<double> msg[msgToSend.size()];
				copy(msgToSend.begin(), msgToSend.end(), msg);

				// Send the array to the intended node, MPI_TAG = tamanho da mensagem
				MPI_Send(msg,
						 msgToSend.size(),
						 MPI_DOUBLE_COMPLEX,
						 node,
						 msgToSend.size(),
						 MPI_COMM_WORLD);

				msgToSend.clear();
			}
		}
	}

	// envia mensagem -1 para todos os ranks que nao receberam uma operacao
	complex<double> end = -1;
	for(size_t i = 0; i < ranks.size(); i++) {
		// exceto a ele proprio
		if(ranks[i] == this->rank)
			continue;

		MPI_Send(&end, 1, MPI_DOUBLE_COMPLEX, ranks[i], 0, MPI_COMM_WORLD);
	}

	// Receber todas as mensagens

	// Constroi um vetor com as operacoes recebidas
	vector<complex<double>> receivedOperations;

	MPI_Status status;
	int MPI_RECV_BUFFER_SIZE = 10;
	complex<double> msg[MPI_RECV_BUFFER_SIZE];
	msg[0] = 0;
	for(int node = 0; node < this->size; node++) {
		// exceto a dele proprio
		if(node == this->rank)
			continue;

		MPI_Recv(&msg,
				 MPI_RECV_BUFFER_SIZE,
				 MPI_DOUBLE_COMPLEX,
				 node,
				 MPI_ANY_TAG,
				 MPI_COMM_WORLD,
				 &status);

		// Se mensagem for de uma operacao
		if(status.MPI_TAG != 0) {
			// cout << "Received Ops: " << endl;
			for(int i = 0; i < status.MPI_TAG; i++) {
				// cout << msg[i] << endl;
				receivedOperations.push_back(msg[i]);
			}
		}
	}

	return receivedOperations;
}

int QubitLayerMPI::getNodeOfState(unsigned long state)
{
	return floor(state / (this->states.size() / 2));
}

void QubitLayerMPI::measure()
{
	int i = this->rank * this->states.size();
	size_t j = 0;

	while(j < this->states.size()) {
		float result = pow(abs(this->states[j]), 2); // not sure...

		appendResultLog("Node ",
						rank,
						": |",
						bitset<numQubitsMPI>(i / 2).to_string(),
						"> -> ",
						result,
						"\n");

		i += 2;
		j += 2;
	}

	// Print logs orderly
	for(int i = 0; i < size; i++) {
		if(rank == i)
			cout << resultLog.str() << endl;
		else
			MPI_Barrier(MPI_COMM_WORLD);
	}
}

void QubitLayerMPI::toffoli(int controlQubit1, int controlQubit2, int targetQubit)
{
	// Executes pauliX if both control qubits are set to |1>
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i;
			if(state.test(controlQubit1) && state.test(controlQubit2)) {
				bitset<numQubitsMPI> state = i;
				state.flip(targetQubit);
				this->states[2 * state.to_ulong() + 1] = this->states[2 * i];
			} else {
				this->states[2 * i + 1].real(this->states[2 * i].real());
			}
		}
	}
	updateStates();
}

void QubitLayerMPI::controlledX(int controlQubit, int targetQubit)
{
	// Executes pauliX if control qubit is |1>
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i;
			if(state.test(controlQubit)) {
				bitset<numQubitsMPI> state = i;
				state.flip(targetQubit);
				this->states[2 * state.to_ulong() + 1] = this->states[2 * i];
			} else {
				this->states[2 * i + 1].real(this->states[2 * i].real());
			}
		}
	}
	updateStates();
}

void QubitLayerMPI::controlledZ(int controlQubit, int targetQubit)
{
	// Executes pauliZ if control qubit is |1>

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i;
			if(state.test(controlQubit)) {
				state[targetQubit] != 0
					? this->states[2 * i + 1].real(this->states[2 * i].real() * -1)
					: this->states[2 * i + 1].real(this->states[2 * i].real());
			} else {
				this->states[2 * i + 1].real(this->states[2 * i].real());
			}
		}
	}
	updateStates();
}

bool QubitLayerMPI::checkZeroState(int i)
{
	return this->states[i * 2].real() != 0 || this->states[i * 2].imag() != 0;
}

void QubitLayerMPI::hadamard(int targetQubit)
{
	vector<complex<double>> statesOOB;

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i;
			if(state.test(targetQubit)) {
				this->states[2 * i + 1] -= (1 / sqrt(2)) * this->states[2 * i];
			} else {
				this->states[2 * i + 1] += (1 / sqrt(2)) * this->states[2 * i];
			}
		}
	}

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i + (rank * this->states.size() / 2);
			state.flip(targetQubit);

			if(!checkStateOOB(state)) {
				int localIndex =
					state.to_ulong() - (rank * (this->states.size() / 2));
				this->states[2 * localIndex + 1] +=
					(1 / sqrt(2)) * this->states[2 * i];
			} else {

#ifdef HADAMARD_DEBUG_LOGS
				appendDebugLog("Hadamard: State |", state, "> out of bounds!\n");
#endif

				// pair (state, intended_value)
				statesOOB.push_back(state.to_ulong());
				statesOOB.push_back(this->states[2 * i]);
			}
		}
	}

	vector<complex<double>> receivedOps = handlerStatesOOB(statesOOB);

	for(size_t i = 0; i < receivedOps.size(); i += 2) {
		// calcula o index local do state recebido
		int localIndex =
			receivedOps[i].real() - (this->rank * (this->states.size() / 2));

		this->states[2 * localIndex + 1] += (1 / sqrt(2)) * receivedOps[i + 1];
	}

	updateStates();
}

void QubitLayerMPI::pauliZ(int targetQubit)
{
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i + (rank * this->states.size() / 2);

			state[targetQubit] != 0
				? this->states[2 * i + 1].real(this->states[2 * i].real() * -1)
				: this->states[2 * i + 1].real(this->states[2 * i].real());

#ifdef PAULIZ_DEBUG_LOGS
			appendDebugLog("State vector before update: ", getStateVector());
#endif
		}
	}
	updateStates();

#ifdef PAULIZ_DEBUG_LOGS
	appendDebugLog("State vector after update: ", getStateVector());
#endif
}

void QubitLayerMPI::pauliY(int targetQubit)
{
	// vector of (stateOTB, value) pairs
	vector<complex<double>> statesOOB;

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i + (rank * this->states.size() / 2);
			// if |0>, scalar 1i applies to |1>
			// if |1>, scalar -1i applies to |0>
			// probabily room for optimization here
			state.flip(targetQubit);

			if(!checkStateOOB(state)) {
				int localIndex =
					state.to_ulong() - (rank * (this->states.size() / 2));

				state[targetQubit] == 0
					? this->states[2 * localIndex + 1] = this->states[2 * i] * 1i
					: this->states[2 * localIndex + 1] = this->states[2 * i] * -1i;

			} else {

#ifdef PAULIY_DEBUG_LOGS
				appendDebugLog("PauliY: State |", state, "> out of bounds!\n");

#endif

				// pair (state, intended_value)
				statesOOB.push_back(state.to_ulong());
				statesOOB.push_back(this->states[2 * i]);
			}
		}
	}

	vector<complex<double>> receivedOps = handlerStatesOOB(statesOOB);

	for(size_t i = 0; i < receivedOps.size(); i += 2) {
		// calcula o index local do state recebido
		int localIndex =
			receivedOps[i].real() - (this->rank * (this->states.size() / 2));

		this->states[2 * localIndex + 1].real() == 0
			? this->states[2 * localIndex + 1] = receivedOps[i + 1] * 1i
			: this->states[2 * localIndex + 1] = receivedOps[i + 1] * -1i;
	}

	updateStates();
}

void QubitLayerMPI::pauliX(int targetQubit)
{
	// vector of (stateOTB, value) pairs
	vector<complex<double>> statesOOB;

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i + (rank * this->states.size() / 2);
			state.flip(targetQubit);

			// if a state is OTB, store tuple (state, intended_value) to a vector
			if(!checkStateOOB(state)) {
				int localIndex =
					state.to_ulong() - (rank * (this->states.size() / 2));
				this->states[2 * localIndex + 1] = this->states[2 * i];

			} else {

#ifdef PAULIX_DEBUG_LOGS
				appendDebugLog("PauliX: State |", state, "> out of bounds!\n");
#endif

				// pair (state, intended_value)
				statesOOB.push_back(state.to_ulong());
				statesOOB.push_back(this->states[2 * i]);
			}
		}
	}

	vector<complex<double>> receivedOps = handlerStatesOOB(statesOOB);

	for(size_t i = 0; i < receivedOps.size(); i += 2) {
		// calcula o index local do state recebido
		int localIndex =
			receivedOps[i].real() - (this->rank * (this->states.size() / 2));

		// operacao especifica ao pauliX
		this->states[2 * localIndex + 1] = receivedOps[i + 1];
	}

	updateStates();
}

void QubitLayerMPI::updateStates()
{
	for(size_t i = 0; i < this->states.size(); i += 2) {
		this->states[i] = this->states[i + 1];
		this->states[i + 1] = {0, 0};
	}
}

string QubitLayerMPI::getStateVector()
{
	stringstream stateVector;
	for(size_t i = 0; i < this->states.size(); i++) {
		stateVector << this->states[i];

		if(i % 2 == 1)
			stateVector << " | ";
	}
	stateVector << "\n";
	return stateVector.str();
}

void QubitLayerMPI::printStateVector()
{
	for(size_t i = 0; i < this->states.size(); i++) {
		cout << this->states[i];

		if(i % 2 == 1)
			cout << " | ";
	}
	cout << endl << endl;
}

QubitLayerMPI::QubitLayerMPI(size_t qLayerSize, int rank, int size)
{
	this->rank = rank;
	this->size = size;
	// populate vector with all (0,0)
	size_t i = 0;
	while(i < qLayerSize) {
		this->states.push_back(0);
		++i;
	}

	appendDebugLog(
		"\n--------------- Process ", to_string(rank), " logs --------------- \n");
}