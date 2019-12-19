#ifndef _NewtonAnimationBind_
#define _NewtonAnimationBind_

#include <dList.h>
#include <dVector.h>

class NewtonAnimationPose;
class NetwonPoseTransform;

typedef void (*PosetUpdateCallback) (void* useData, dFloat* posit, dFloat* rotation);

class NewtonBindFrameToNode
{
    public:
    void* m_userData;
    NetwonPoseTransform* m_sourceTranform;
};

class NewtonAnimationPoseBind: public dList<NewtonBindFrameToNode>
{
    public:
    NewtonAnimationPoseBind(NewtonAnimationPose* pose);
    void Release ();

    void UpdatePose() const;
    void GeneratePose (dFloat param);
    void SetUpdateCallback (PosetUpdateCallback callback);
    
    protected:
    ~NewtonAnimationPoseBind(void);

    int m_refCount;
    NewtonAnimationPose* m_pose;
    PosetUpdateCallback m_updateCallback;
};


#endif