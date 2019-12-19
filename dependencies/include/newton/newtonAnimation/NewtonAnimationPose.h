#ifndef _NewtonAnimationPose_
#define _NewtonAnimationPose_

#include <dList.h>
#include <dVector.h>
#include <dQuaternion.h>

class NewtonKeyFrames;
class NewtonAnimationClip;
class NewtonAnimationPoseBind;



class NetwonPoseTransform
{
    public:
    dQuaternion m_rotation;
    dFloat m_posit[3];
    NewtonKeyFrames* m_source;
};

class NewtonAnimationPose: public dList<NetwonPoseTransform>
{
    public:
    NewtonAnimationPose(NewtonAnimationClip* clip);
    void Release ();

    void Generate (dFloat param);

    protected:
    ~NewtonAnimationPose(void);
    int m_refCount;
    NewtonAnimationClip* m_clip;

    friend NewtonAnimationPoseBind;
};


#endif