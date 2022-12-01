#include "Subroutine.h"
#include "Actors/PlayerCharacter.h"

void ScriptSubroutine::OnCall()
{
    if (Parent)
    {
        Parent->Player->Analyzer->Analyze((char*)OffsetAddress, Parent);
    }
}