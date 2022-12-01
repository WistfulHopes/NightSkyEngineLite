#pragma once
#pragma pack (push, 1)

enum EBoxType
{
	Hurtbox,
	Hitbox,
};

struct CollisionBox
{
	EBoxType Type;
	int PosX;
	int PosY;
	int SizeX;
	int SizeY;

	bool operator!=(const CollisionBox& OtherBox)
	{
		return this->Type != OtherBox.Type || this->PosX != OtherBox.PosX || this->PosY != OtherBox.PosY
			|| this->SizeX != OtherBox.SizeX || this->SizeY != OtherBox.SizeY;
	}
};
#pragma pack(pop)