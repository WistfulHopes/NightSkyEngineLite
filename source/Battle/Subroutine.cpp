#include "Subroutine.h"
#include "Actors/PlayerCharacter.h"

void ScriptSubroutine::OnCall()
{
    if (CommonSubroutine)
    {
        Parent->Player->CommonAnalyzer->Analyze((char*)OffsetAddress, Parent);
        return;
    }
    if (Parent)
    {
        Parent->Player->CharaAnalyzer->Analyze((char*)OffsetAddress, Parent);
    }
}