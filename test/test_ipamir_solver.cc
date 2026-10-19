#include "../src/server/ipamir_solver.cc"
#include <cassert>
#include <iostream>

void testInitializeReleaseIPAMIR() {
    void* solver = initializeIPAMIR();
    assert(solver != nullptr); // Stellt sicher, dass der Solver initialisiert wurde
    releaseIPAMIR(solver); // Gibt die Ressourcen des Solvers frei
    std::cout << "testInitializeReleaseIPAMIR passed." << std::endl;
}

void testAddHardClause() {
    void* solver = initializeIPAMIR();
    std::vector<int> clause = {1, -2, 3};
    addHardClause(solver, clause);
    // Hier sollte eine Methode zum Überprüfen der Klausel folgen
    releaseIPAMIR(solver);
    std::cout << "testAddHardClause passed." << std::endl;
}

void testAddSoftClause() {
    void* solver = initializeIPAMIR();
    std::vector<int> literals = {4, -5};
    uint64_t weight = 10;
    addSoftClause(solver, literals, weight);
    // Hier sollte eine Methode zum Überprüfen der Soft-Klausel folgen
    releaseIPAMIR(solver);
    std::cout << "testAddSoftClause passed." << std::endl;
}

void testAddAssumptionSolve() {
    void* solver = initializeIPAMIR();
    std::vector<int> clause = {1, 2};
    addHardClause(solver, clause);
    addAssumption(solver, 1);
    addAssumption(solver, -2);
    solveIPAMIR(solver);
    int32_t value_1 = extractLiteralValue(solver, 1);
    int32_t value_2 = extractLiteralValue(solver, 2);
    assert(value_1 == 1);
    assert(value_2 == -2);

    addAssumption(solver, -1);
    addAssumption(solver, 2);
    solveIPAMIR(solver);
    value_1 = extractLiteralValue(solver, 1);
    value_2 = extractLiteralValue(solver, 2);
    assert(value_1 == -1);
    assert(value_2 == 2);
    releaseIPAMIR(solver);
    std::cout << "testAddAssumptionSolve passed." << std::endl;
}

void testComputeSolutionCost() {
    void* solver = initializeIPAMIR();
    // Beispiel Soft-Klausel hinzufügen und lösen
    addHardClause(solver, {1});
    addSoftClause(solver, {-1}, 10);
    solveIPAMIR(solver);
    uint64_t cost = computeSolutionCost(solver);
    assert(cost == 10);
    releaseIPAMIR(solver);
    std::cout << "testComputeSolutionCost passed." << std::endl;
}

void testExtractLiteralValue() {
    void* solver = initializeIPAMIR();
    addHardClause(solver, {1});
    addHardClause(solver, {-2});
    solveIPAMIR(solver);
    int32_t value_1 = extractLiteralValue(solver, 1);
    int32_t value_2 = extractLiteralValue(solver, 2);
    assert(value_1 == 1);
    assert(value_2 == -2);
    releaseIPAMIR(solver);
    std::cout << "testExtractLiteralValue passed." << std::endl;
}

void testSolveIPAMIR() {
    void* solver = initializeIPAMIR();
    addHardClause(solver, {1});
    int result = solveIPAMIR(solver);
    assert(result == 30);
    addHardClause(solver, {-1});
    result = solveIPAMIR(solver);
    assert(result == 10);
    releaseIPAMIR(solver);
    std::cout << "testSolveValue passed." << std::endl;
}

int main() {
    testInitializeReleaseIPAMIR();
    testAddHardClause();
    testAddSoftClause();
    testAddAssumptionSolve();
    testComputeSolutionCost();
    testExtractLiteralValue();
    testSolveIPAMIR();
    return 0;
}
