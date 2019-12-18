#ifndef __SYM_SPRITE
#define __SYM_SPRITE

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include <string>
#include <map>

#include "SYM_Maths.h"
#include "SYM_Quaternion.h"
#include "SYM_Texture.h"
#include "SYM_Material.h"

namespace sym
{
    class CSprite
    {
        public:
            //Constructor
            CSprite();
            //~CSprite();

            //Load texture
            bool Load(std::string Filename);
			bool Load(CTexture *Tex);

            //Render sprite
            void Render();

            //Transformation
            void SetPosition(CVector3 Pos);
            void SetRotation(CQuaternion Rot);
            void SetScale(CVector3 Scl);
			void SetSize(CVector3 Sze) { Size = Sze; }
			void UpdateBoundingBox();

			const CVector3 GetPosition() { return Position; }
			const CQuaternion GetRotation() { return Rotation; }
			const CVector3 GetSize() { return Size; }
			const RECT GetBoundingBox() { return BoundingBox; }

			//Collision detection
			bool CheckCollision(CVector3 Point);
			bool CheckCollision(CSprite *Sprite);

            //Colour
            void SetTransparency(float Trans);
			void SetRenderTint(CColour Colour) { Tint = Colour; }

			//Set depth test
			void SetDepthTest(bool Depth);

			//Draw both sides
			void SetDoubleSided(bool Sides) { DoubleSided = Sides; }

        private:
            //Texture
            CTexture *Texture;

            //Transformation
            CVector3 Position;
            CQuaternion Rotation;
            CVector3 Scale;
            CVector3 Size;
			RECT BoundingBox;

            //Transparency
            float Transparency;
			CColour Tint;

			//Do depth test
			bool DepthTest;

			//Draw both sides
			bool DoubleSided;
    };

    class CSpriteManager
    {
        public:
            CSpriteManager();
            ~CSpriteManager();

            //Load sprite
            bool LoadSprite(std::string Name, std::string Filename);

            //Get sprite by name
            CSprite *GetSprite(std::string Name);

        private:
            //Map of sprites
            std::map<std::string, CSprite*> Sprites;
    };

} //namespace

#endif // __SYM_SPRITE
