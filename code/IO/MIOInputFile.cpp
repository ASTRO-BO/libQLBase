#include "InputFile.h"

namespace ql_io {

InputFile::InputFile() {
	opened = kFALSE;
	status = 0;
	applyFilter = kFALSE;
	selectEvent = "";
	headerBase = 1; //starting index = 0
}


InputFile::~InputFile() {

}

void InputFile::SetBaseHeader(Int_t headerBase) {
	this->headerBase = headerBase;
}


Bool_t InputFile::IsOpened() {
	return opened;
}

Long_t InputFile::GetNEvents() {
	MoveHeader(headerBase);
	return GetNRows();
}

}
