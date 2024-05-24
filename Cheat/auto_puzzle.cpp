#include "auto_puzzle.h"

#include "engine.h"
#include "globals.h"
#include "hooks.h"

#include "utils.h"

#include "imgui_internal.h"

namespace AutoPuzzle
{
	namespace CablePuzzle
	{
		static bool LADNBFDOAMJHandler(RPG::Client::Prop::CablePuzzleBoard* _this, void* FMJLOIDBOKI, void* BHMGPLJEDCH)
		{
			if (!Options.bAutoPuzzle)
				return CALL_ORIGIN(LADNBFDOAMJHandler, _this, FMJLOIDBOKI, BHMGPLJEDCH);

			return TRUE;
		}
	}

	namespace RubikCubePuzzle
	{
		static void OnRotateFinishHandler(RPG::Client::Prop::RubikCubePuzzleCube* _this)
		{
			if (Options.bAutoPuzzle && _this)
				_this->Order = 0;

			CALL_ORIGIN(OnRotateFinishHandler, _this);
		}
	}

	namespace RotatePillarPuzzle
	{
#pragma optimize("", off)
		static bool get_IsCompleteHandler(void* _this)
		{
			if (!Options.bAutoPuzzle)
				return CALL_ORIGIN(get_IsCompleteHandler, _this);

			return TRUE;
		}
#pragma optimize("", on)
	}

	namespace InsertionPuzzle
	{
		static bool IsMatchItemHandler(void* _this, void* FBLKNKFEIFI)
		{
			if (!Options.bAutoPuzzle)
				return CALL_ORIGIN(IsMatchItemHandler, _this, FBLKNKFEIFI);

			return TRUE;
		}
	}

	namespace OpticalIllusionPuzzle
	{
		static void UpdateHandler(RPG::Client::Prop::OpticalIllusionPuzzleBoard* _this)
		{
			CALL_ORIGIN(UpdateHandler, _this);

			if (Options.bAutoPuzzle && _this)
			{
				_this->DesignPaths->max_length = 0; // why not

				if (_this->KDHABOGLJKC != RPG::Client::Prop::PuzzlePhase_FinishDelay)
					RPG::Client::Prop::OpticalIllusionPuzzleBoard::MELEDIOAKMC(_this);
			}
		}
	}

	namespace JigsawPuzzle
	{
#pragma optimize("", off)
		static bool CheckIsGameFinishHandler(void* _this)
		{
			if (!Options.bAutoPuzzle)
				return CALL_ORIGIN(CheckIsGameFinishHandler, _this);

			return TRUE;
		}
#pragma optimize("", on)
	}

	static void LockHandler(void* _this, void* lockSource, void* lockParams)
	{
		return;
	}

	void Render()
	{
		ImGui::BeginGroupPanel("Auto-Puzzle");

		ImGui::Checkbox("Enable", &Options.bAutoPuzzle);

		ImGui::SameLine();

		ImGui::HelpMarker("In some puzzles you need to make the first move yourself.");

		ImGui::EndGroupPanel();
	}

	void Update()
	{

	}

	void Start()
	{
		CreateHook(RPG::Client::Prop::CablePuzzleBoard::LADNBFDOAMJ, CablePuzzle::LADNBFDOAMJHandler);

		CreateHook(RPG::Client::Prop::RubikCubePuzzleCube::OnRotateFinish, RubikCubePuzzle::OnRotateFinishHandler);
		CreateHook(RPG::Client::Prop::RotatePillarPuzzleFragment::get_IsComplete, RotatePillarPuzzle::get_IsCompleteHandler);
		CreateHook(RPG::Client::Prop::InsertionPuzzleItem::IsMatchItem, InsertionPuzzle::IsMatchItemHandler);

		CreateHook(RPG::Client::Prop::OpticalIllusionPuzzleBoard::Update, OpticalIllusionPuzzle::UpdateHandler);
		CreateHook(RPG::Client::Prop::JigsawPuzzleBoard::_CheckIsGameFinish, JigsawPuzzle::CheckIsGameFinishHandler);

		CreateHook(RPG::Client::GamePlayLockModule::Lock, LockHandler);
	}
}