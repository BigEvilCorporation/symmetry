#include "NewtonAnimationStdAfx.h"
#include "NewtonAnimationPoseBind.h"
#include "NewtonAnimationPose.h"
#include "NewtonAnimationClip.h"


NewtonAnimationPoseBind::NewtonAnimationPoseBind(NewtonAnimationPose* pose)
{
	m_refCount = 1;
	m_pose = pose;
	pose->m_refCount ++;
	m_updateCallback = NULL;

	for (NewtonAnimationPose::dListNode* node = m_pose->GetFirst(); node; node = node->GetNext()) {
		Append ();
		NewtonBindFrameToNode& poseBind = GetLast()->GetInfo();
		poseBind.m_userData = NULL;
		poseBind.m_sourceTranform = &node->GetInfo();
	}
}

NewtonAnimationPoseBind::~NewtonAnimationPoseBind(void)
{

}

void NewtonAnimationPoseBind::Release ()
{
	m_refCount --;
	if (!m_refCount) {
		delete this;
	}
}

void NewtonAnimationPoseBind::SetUpdateCallback (PosetUpdateCallback callback)
{
	m_updateCallback = callback;
}


void NewtonAnimationPoseBind::GeneratePose (dFloat param)
{
	m_pose->Generate (param);
}


void NewtonAnimationPoseBind::UpdatePose() const
{
	if (m_updateCallback) {
		for (dListNode* node = GetFirst(); node; node = node->GetNext()) {
			NewtonBindFrameToNode& bindFrame = node->GetInfo();
			m_updateCallback (bindFrame.m_userData, &bindFrame.m_sourceTranform->m_posit[0], &bindFrame.m_sourceTranform->m_rotation.m_q0); 
		}
	}
}