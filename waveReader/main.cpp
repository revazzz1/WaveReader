#include <iostream>
#include "WaveReader.hh"
#include "Graph.hh"
#include <math.h>

using namespace std;

double getCoef(WaveReader w) {
    double *wave = w.GetData(0);
    double *wave1 = w.GetData(1);
    double voltage = 0.0;
    double current = 0.0;
    double P = 0.0;
    for(int i = 0; i < w.GetNsamples(); i++) {
        current += wave[i] * wave[i];
        voltage += wave1[i] * wave1[i];
        P += wave[i] * wave1[i];
    }
    P *= (-1.0);
    double S = sqrt(current*voltage);
    cout << "Voltage sum: " << voltage << endl << "Current sum: " << current << endl << "P: " << P << "S: " << S << "coef: " << P/S << endl;

    return P/S;
}

int main(){
    WaveReader w("load.wav",5000);

    w.Print();
    w.SetVpp(20);

    Graph graph1("Current");
    Graph graph2("Voltage");

    double *wave = w.GetData(0);
    double *wave1 = w.GetData(1);
    double * time = w.GetTimeAxis();


    for(int i = 0; i < w.GetNsamples(); i++){
        graph1.AddPoint(time[i],wave[i],0);
        graph2.AddPoint(time[i],wave1[i],0);
    }

   cout << "coef is: "<< getCoef(w) << endl;

    graph1.draw();
    graph2.draw();
}
