#pragma once
#include "Buffer.h"
#include "Slice.h"

// > 0 ������������Ϣ����Ϣ����msg�У�������ɨ����ֽ���
// == 0 ����������Ϣ
// < 0 ��������
struct CodecBase 
{
	virtual int TryDecode(Slice data, Slice& msg) = 0;
	virtual void Encode(Slice msg, Buffer& buf) = 0;
	virtual CodecBase* Clone() = 0;
};

//�������ȵ���Ϣ
struct LengthCodec :public CodecBase 
{
	int TryDecode(Slice data, Slice& msg) override;
	//���len+msg
	void Encode(Slice msg, Buffer& buf) override;
	CodecBase* Clone() override { return new LengthCodec(); }
};
