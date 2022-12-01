#include "State.h"
#include "Actors/PlayerCharacter.h"

void ScriptState::OnEnter()
{
    if (Offsets.OnEnterOffset == -1)
        return;
    if (ParentState)
    {
        ParentState->OnEnter();
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnEnterOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnEnterOffset, Parent);
    }
}

void ScriptState::OnUpdate(float DeltaTime)
{
    if (Offsets.OnUpdateOffset == -1)
        return;
    if (ParentState)
    {
        ParentState->OnUpdate(DeltaTime);
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnUpdateOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnUpdateOffset, Parent);
    }
}

void ScriptState::OnExit()
{
    if (Offsets.OnExitOffset == -1)
        return;
    if (ParentState)
    {
        ParentState->OnExit();
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnExitOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnExitOffset, Parent);
    }
}

void ScriptState::OnLanding()
{
    if (Offsets.OnLandingOffset == -1)
        return;
    if (ParentState)
    {
        ParentState->OnLanding();
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnLandingOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnLandingOffset, Parent);
    }
}

void ScriptState::OnHit()
{
    if (Offsets.OnHitOffset == -1)
        return;
    if (ParentState)
    {
        ParentState->OnHit();
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnHitOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnHitOffset, Parent);
    }
}

void ScriptState::OnBlock()
{
    if (Offsets.OnBlockOffset == -1)
        return;
    if (ParentState)
    {
        ParentState->OnBlock();
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnBlockOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnBlockOffset, Parent);
    }
}

void ScriptState::OnHitOrBlock()
{
    if (Offsets.OnHitOrBlockOffset == -1)
        return;
    if (ParentState)
    {
        ParentState->OnHitOrBlock();
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnHitOrBlockOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnHitOrBlockOffset, Parent);
    }
}

void ScriptState::OnCounterHit()
{
    if (Offsets.OnCounterHitOffset == -1)
        return;
    if (ParentState)
    {
        ParentState->OnCounterHit();
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnCounterHitOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnCounterHitOffset, Parent);
    }
}

void ScriptState::OnSuperFreeze()
{
    if (Offsets.OnSuperFreezeOffset == -1)
        return;
    if (ParentState)
    {
        ParentState->OnSuperFreeze();
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnSuperFreezeOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnSuperFreezeOffset, Parent);
    }
}

void ScriptState::OnSuperFreezeEnd()
{
    if (Offsets.OnSuperFreezeEndOffset == -1)
        return;
    if (ParentState)
    {
        ParentState->OnSuperFreezeEnd();
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnSuperFreezeEndOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnSuperFreezeEndOffset, Parent);
    }
}