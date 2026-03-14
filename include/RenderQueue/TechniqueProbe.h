#pragma once

namespace DynamicConstBuf
{
	class Buffer;
}

class TechniqueProbe
{
public:
	void SetTechnique(class Technique* pTech_in)
	{
		pTech = pTech_in;
		techIdx++;
		OnSetTechnique();
	}
	void SetStep(class Step* pStep_in)
	{
		pStep = pStep_in;
		stepIdx++;
		OnSetStep();
	}
	virtual bool VisitBuffer(class DynamicConstBuf::Buffer& buf)
	{
		bufIdx++;
		return OnVisitBuffer(buf);
	}
protected:
	virtual void OnSetTechnique() {}
	virtual void OnSetStep() {}
	virtual bool OnVisitBuffer(class DynamicConstBuf::Buffer&) = 0;
protected:
	class Technique* pTech;
	class Step* pStep;
	size_t techIdx = std::numeric_limits<size_t>::max();
	size_t stepIdx = std::numeric_limits<size_t>::max();
	size_t bufIdx = std::numeric_limits<size_t>::max();
};