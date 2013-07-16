#include "MIOInputFile.h"

MIOInputFile::MIOInputFile() {
	opened = kFALSE;
	status = 0;
	applyFilter = kFALSE;
	selectEvent = "";
	headerBase = 1; //starting index = 0
}


MIOInputFile::~MIOInputFile() {

}

void MIOInputFile::SetBaseHeader(Int_t headerBase) {
	this->headerBase = headerBase;
}


Bool_t MIOInputFile::IsOpened() {
	return opened;
}

Long_t MIOInputFile::GetNEvents() {
	MoveHeader(headerBase);
	return GetNRows();
}
