#ifndef _NewtonAnimationClip_
#define _NewtonAnimationClip_

#include <dList.h>
#include <dVector.h>
#include <dQuaternion.h>

class TiXmlElement;
class NewtonAnimationPose;

class NewtonKeyFrames
{
    public:
    NewtonKeyFrames();
    ~NewtonKeyFrames();

    void LoadKeyFrames(const TiXmlElement* keyFrames);

    char m_bindName[32];
    int m_keyFrameCounts;
    int* m_keys;
    dVector* m_posit;
    dQuaternion* m_rotation;
};

class NewtonAnimationClip : public dList<NewtonKeyFrames>
{
public:
	NewtonAnimationClip(void);
	void Release ();
	

	void Load (const char* name);
	void Save (const char* name) const;

	void RemoveNode (const char* name);

	protected:
	~NewtonAnimationClip(void);

	int m_refCount;
	int m_framesCount;

	friend NewtonAnimationPose;
};


#endif