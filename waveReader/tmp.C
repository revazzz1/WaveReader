
int OpenWave()
{
  //  ifstream inFile("rec.wav", ios::binary);
  ifstream inFile("load.wav", ios::binary);
  //ifstream inFile("Test_Tones/Test_8k_8_400Hz_2.wav", ios::binary);

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

  short NumOfChannels;
  inFile.read((char*)&NumOfChannels, 2);

  Int_t SampleRate;
  inFile.read((char*)&SampleRate, 4);

  int ByteRate;
  inFile.read((char*)&ByteRate, 4);

  short BlockAlign;
  inFile.read((char*)&BlockAlign, 2);

  short BitsPerSample;
  inFile.read((char*)&BitsPerSample, 2);

  char Subchunk2ID[4];
  inFile.read(Subchunk2ID, 4);
  if (!strstr(Subchunk2ID,"data")) return -4;
  
  int Subchunk2Size;
  inFile.read((char*)&Subchunk2Size, 4);
  int NumOfSamples = Subchunk2Size*8/BitsPerSample/NumOfChannels;

  cout << "NumOfChannels = " << NumOfChannels << endl;
  cout << "SampleRate = " << SampleRate << " Hz" << endl;
  cout << "BitsPerSample = " << BitsPerSample << endl;
  cout << "Number of Samples = " << NumOfSamples << endl;

  float **Channel;
  float *Time;

  try {
    Time  = new float [NumOfSamples];
    Channel = new float* [NumOfChannels];
    for (int k=0; k<NumOfChannels; k++) Channel[k] = new float[NumOfSamples];
  }
  catch (bad_alloc&) {
    cout << "Error allocating memory: No enough free memory!" << endl;
    return -5;
  }

  int data;
  int MaxRange = pow(2,BitsPerSample);
  int MaxPositive = MaxRange/2 - 1;
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

  return 0;
}
