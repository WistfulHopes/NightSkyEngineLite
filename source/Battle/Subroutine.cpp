#include "Subroutine.h"
#include "Actors/PlayerCharacter.h"

void ScriptSubroutine::OnCall()
{
    if (Parent)
    {
        Parent->Player->CharaAnalyzer->Analyze((char*)OffsetAddress, Parent);
    }
}