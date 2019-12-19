#include "NewtonAnimationStdAfx.h"
#include "NewtonAnimationClip.h"
#include "tinyxml.h"


#if (_MSC_VER >= 1400)
#pragma warning (disable: 4996) // for 2005 users declared deprecated
#pragma warning (disable: 4100) // unreferenced formal parameter
#endif


NewtonAnimationClip::NewtonAnimationClip(void)
{
	m_refCount = 1;
}

NewtonAnimationClip::~NewtonAnimationClip(void)
{
}


void NewtonAnimationClip::Release ()
{
	m_refCount --;
	if (!m_refCount) {
		delete this;
	}
}

void NewtonAnimationClip::Save (const char* name) const
{
}

void NewtonAnimationClip::RemoveNode (const char* name)
{
	for (dListNode* node = GetFirst(); node; node = node->GetNext()) {        
		if (!strcmp (node->GetInfo().m_bindName, name)) {
			Remove (node);
			break;
		}
	}
}


void NewtonAnimationClip::Load (const char* fileName)
{
	const TiXmlElement* root;
	TiXmlDocument doc (fileName);
	doc.LoadFile();

	root = doc.RootElement();
	if (root && !strcmp (root->Attribute ("vendor"), "newton 2.0 file format")){
		const TiXmlElement* anim;

		anim = root->FirstChildElement ("MeshAnimation");
		if (anim) {

			anim->Attribute ("framesCount", &m_framesCount);  
			for (const TiXmlElement* keyFrames = (TiXmlElement*)anim->FirstChild("node"); keyFrames; keyFrames = (TiXmlElement*)keyFrames->NextSibling()) {
				NewtonKeyFrames* node;
				Append ();
				node = &GetLast()->GetInfo();
				node->LoadKeyFrames(keyFrames);
			}
		}
	}
}


NewtonKeyFrames::NewtonKeyFrames()
{
	m_keys = NULL;
	m_posit = NULL;
	m_rotation = NULL;

}

NewtonKeyFrames::~NewtonKeyFrames()
{
	if (m_keys) {
		delete[] m_keys;
		delete[] m_posit;
		delete[] m_rotation;
	}
}


void NewtonKeyFrames::LoadKeyFrames(const TiXmlElement* keyFrames)
{
	sprintf (m_bindName, "%s", keyFrames->Attribute ("name"));
	keyFrames->Attribute ("keyFramesCount", &m_keyFrameCounts);  

	m_keys = new int[m_keyFrameCounts];
	m_posit = new dVector[m_keyFrameCounts];
	m_rotation = new dQuaternion[m_keyFrameCounts];


	const char* keyFrameData;
	TiXmlElement* keyFramesSrc;
	keyFramesSrc = (TiXmlElement*)keyFrames->FirstChild("keys");
	keyFrameData = keyFramesSrc->Attribute ("int");  
	for (int i = 0; i < m_keyFrameCounts; i ++) {
		char buffer[32];
		const char *ptr;
		ptr = strchr (keyFrameData, ' ');
		strncpy (buffer, keyFrameData, ptr - keyFrameData);
		buffer[ptr - keyFrameData] = 0;
		m_keys[i] = atoi (buffer);
		keyFrameData = ptr + 1;
	}

	keyFramesSrc = (TiXmlElement*)keyFrames->FirstChild("positions");
	keyFrameData = keyFramesSrc->Attribute ("float3");  
	for (int i = 0; i < m_keyFrameCounts; i ++) {
		char buffer[32];
		const char *ptr;
		ptr = strchr (keyFrameData, ' ');
		strncpy (buffer, keyFrameData, ptr - keyFrameData);
		buffer[ptr - keyFrameData] = 0;
		m_posit[i].m_x = dFloat (atof (buffer));
		keyFrameData = ptr + 1;

		ptr = strchr (keyFrameData, ' ');
		strncpy (buffer, keyFrameData, ptr - keyFrameData);
		buffer[ptr - keyFrameData] = 0;
		m_posit[i].m_y = dFloat (atof (buffer));
		keyFrameData = ptr + 1;

		ptr = strchr (keyFrameData, ' ');
		strncpy (buffer, keyFrameData, ptr - keyFrameData);
		buffer[ptr - keyFrameData] = 0;
		m_posit[i].m_z = dFloat (atof (buffer));
		keyFrameData = ptr + 1;
		m_posit[i].m_w = 0.0f;
	}


	keyFramesSrc = (TiXmlElement*)keyFrames->FirstChild("rotations");
	keyFrameData = keyFramesSrc->Attribute ("float4");  
	for (int i = 0; i < m_keyFrameCounts; i ++) {
		char buffer[32];
		const char *ptr;
		ptr = strchr (keyFrameData, ' ');
		strncpy (buffer, keyFrameData, ptr - keyFrameData);
		buffer[ptr - keyFrameData] = 0;
		m_rotation[i].m_q0 = dFloat (atof (buffer));
		keyFrameData = ptr + 1;

		ptr = strchr (keyFrameData, ' ');
		strncpy (buffer, keyFrameData, ptr - keyFrameData);
		buffer[ptr - keyFrameData] = 0;
		m_rotation[i].m_q1 = dFloat (atof (buffer));
		keyFrameData = ptr + 1;

		ptr = strchr (keyFrameData, ' ');
		strncpy (buffer, keyFrameData, ptr - keyFrameData);
		buffer[ptr - keyFrameData] = 0;
		m_rotation[i].m_q2 = dFloat (atof (buffer));
		keyFrameData = ptr + 1;

		ptr = strchr (keyFrameData, ' ');
		strncpy (buffer, keyFrameData, ptr - keyFrameData);
		buffer[ptr - keyFrameData] = 0;
		m_rotation[i].m_q3 = dFloat (atof (buffer));
		keyFrameData = ptr + 1;
	}
}

