#include "WaveReader.hh"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>

using namespace std;

WaveReader:: WaveReader(){
    DoNormalize = false;
    Init();
}

WaveReader:: WaveReader(const char* file, int maxsmp){
    Init();
    Open(file,maxsmp);
}

WaveReader:: ~WaveReader(){
    Clear();
}

int WaveReader::Open(const char* file, int maxsmp){

  //  ifstream inFile("rec.wav", ios::binary);
  ifstream inFile(file, ios::binary);
  //  ifstream inFile("Test_Tones/Test_8k_8_400Hz_2.wav", ios::binary);

  char name[4];
  inFile.read(name, 4);
  if (!strstr(name,"RIFF")) return -1;

  int ChunkSize;
  inFile.read((char*)&ChunkSize, 4);

  char Format[4];
  inFile.read(Format, 4);
  if (!strstr(Format,"WAVE")) return -2;

  char Subchunk1ID[4];
  inFile.read((char*)&Subchunk1ID, 4);
  if (!strstr(Subchunk1ID,"fmt ")) return -3;

  int Subchunk1Size;
  inFile.read((char*)&Subchunk1Size, 4);

  short AudioFormat;
  inFile.read((char*)&AudioFormat, 2);

  inFile.read((char*)&NumOfChannels, 2);

  inFile.read((char*)&SampleRate, 4);

  int ByteRate;
  inFile.read((char*)&ByteRate, 4);

  short BlockAlign;
  inFile.read((char*)&BlockAlign, 2);

  inFile.read((char*)&BitsPerSample, 2);

  char Subchunk2ID[4];
  inFile.read(Subchunk2ID, 4);
  if (!strstr(Subchunk2ID,"data")) return -4;

  int Subchunk2Size;
  inFile.read((char*)&Subchunk2Size, 4);

  if(maxsmp > -1)
     NumOfSamples = maxsmp;
  else {
  NumOfSamples = Subchunk2Size*8/BitsPerSample/NumOfChannels;
  }
  try {
    Time  = new double [NumOfSamples];
    Channel = new double* [NumOfChannels];
    for (int k=0; k<NumOfChannels; k++) Channel[k] = new double[NumOfSamples];
  }
  catch (bad_alloc&) {
    cout << "Error allocating memory: Not enough free memory!" << endl;
    return -5;
  }

  int data;
  long MaxRange = GetMaxRange();
  long MaxPositive = GetMaxPositive();
  int SSize = BitsPerSample/8;
  bool Is2sComplement = BitsPerSample>=16;

  inFile.seekg(44, ios::beg);

  for (int k=0; k<NumOfSamples; k++) {
    for (int i=0; i<NumOfChannels; i++) {
      data = 0;
      inFile.read((char*)&data, SSize);
      if (Is2sComplement) {
	if (data>MaxPositive) Channel[i][k] = data - MaxRange;
	else Channel[i][k] = data;
      }
      else Channel[i][k] = data - MaxRange/2;
    }

    Time[k] = 1./SampleRate*k;
  }

 if (DoNormalize == true)
    Scale(Peak2Peak);

    inFile.close();
  return 0;
}


void WaveReader:: Init() {
    NumOfChannels = 0;
    SampleRate = 0;
    BitsPerSample = 0;
    NumOfSamples = 0;
    Peak2Peak = 2;
    DoNormalize = false;
    Channel = 0x0;
    Time = 0x0;
}

void WaveReader::Clear() {
    if (Channel != 0x0){
        for(int i = 0; i<NumOfChannels; i++){
            delete []Channel[i];
        }
        delete []Channel;
    }
    delete []Time;
    Init();
}

long WaveReader::GetMaxRange() {
    long maxrange = (long)1 << BitsPerSample;
    return maxrange;
}

long WaveReader::GetMaxPositive() {
    long maxpositive = ((long)1<<(BitsPerSample-1))-1;
    return maxpositive;
}

double* WaveReader::GetData(int channel) const {
    if (channel >= 0 && channel < NumOfChannels) {
        return (Channel[channel]);
    }
    else
        cout << "Invalid parameter" << endl;
}

void WaveReader::Print() const {
    cout << "Number Of Channels: " << GetNchannels() << endl;
    cout << "Sample Rate: " << GetSampleRate() << endl;
    cout << "Number Of Bits: " << GetNbits() << endl;
    cout << "Number Of Samples: " << GetNsamples() << endl;
}

void WaveReader::SetVpp(double Vpp) {
    double coef;
    if(DoNormalize == false) {
    coef = Vpp/(GetMaxRange() - 1);
    }
    else {
    coef = Vpp / Peak2Peak;
    }
    Scale(coef);
    DoNormalize = true;
    Peak2Peak = Vpp;
}

void WaveReader::Scale(double coe) {
    for(int i = 0; i < NumOfSamples; i++){
        for(int j = 0; j < NumOfChannels; j++){
            Channel[j][i] *= coe;
        }
    }
}
