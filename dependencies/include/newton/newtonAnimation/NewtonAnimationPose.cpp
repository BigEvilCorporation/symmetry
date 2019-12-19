#include "NewtonAnimationStdAfx.h"
#include "NewtonAnimationPose.h"
#include "NewtonAnimationClip.h"

NewtonAnimationPose::NewtonAnimationPose(NewtonAnimationClip* clip)
{
	m_refCount = 1;
	m_clip = clip;
	clip->m_refCount ++;

	for (NewtonAnimationClip::dListNode* node = m_clip->GetFirst(); node; node = node->GetNext()) {
		Append ();
		NetwonPoseTransform& pose = GetLast()->GetInfo();
		pose.m_source = &node->GetInfo();
		pose.m_posit[0] = 0.0f;
		pose.m_posit[1] = 0.0f;
		pose.m_posit[2] = 0.0f;
		pose.m_rotation = dQuaternion();
	}
}

NewtonAnimationPose::~NewtonAnimationPose(void)
{
	m_clip->Release();
}

void NewtonAnimationPose::Release ()
{
	m_refCount --;
	if (!m_refCount) {
		delete this;
	}
}

void NewtonAnimationPose::Generate (dFloat param)
{
	// find the inetvale of the clip
	int integerPart;
	dFloat fractionPart;

	param = dMod (param, 1.00001f);
	if (param >= 1.0f) {
		param = 0.99999f;
	}
	fractionPart = param * (m_clip->m_framesCount - 1.0f);
	integerPart = int (dFloor(fractionPart));
	fractionPart -= integerPart;
	for (dListNode* node = GetFirst(); node; node = node->GetNext()) {
		NetwonPoseTransform& transformPose = node->GetInfo();

		const NewtonKeyFrames* const source = transformPose.m_source;

		// future optinization could be done using a binary search
		for (int i = 0; i < source->m_keyFrameCounts; i ++) {
			if (source->m_keys[i] <= integerPart) {

				dVector posit (source->m_posit[i] + (source->m_posit[i + 1] - source->m_posit[i]).Scale (fractionPart));
				transformPose.m_rotation = source->m_rotation[i].Slerp (source->m_rotation[i + 1], fractionPart);
				transformPose.m_posit[0] = posit.m_x;
				transformPose.m_posit[1] = posit.m_y;
				transformPose.m_posit[2] = posit.m_z;
				break;
			}
		}

	}

}