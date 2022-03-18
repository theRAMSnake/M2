#include <iostream>
#include <chrono>
#include <fstream>
#include <Core/ICore3.hpp>
#include <vector>
#include <string>
#include <map>

using namespace std;

int main(int argc,  char** argv)
{
    auto core = materia::createCore({"/home/snake/materia.totest"});

    auto start = chrono::steady_clock::now();

    auto responce = core->executeCommandJson("{\"operation\":\"query\",\"filter\":\"IS(strategy_node) AND ChildOf(8735f22a-1bf2-4a43-9cbf-26f9fee35b8b)\"}");

    auto end = chrono::steady_clock::now();

    cout << chrono::duration_cast<chrono::milliseconds>(end-start).count() << " milliseconds" << endl;
}
