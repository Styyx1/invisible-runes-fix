namespace EDID {
    using _GetFormEditorID = const char *(*)(std::uint32_t);

    /*
    * Given a TESForm, it returns the Editor ID. For forms whose ID is not cached,
    * tries to fetch the ID through PowerOfThree's Tweaks. If this fails, or the form
    * doesn't have an EditorID it returns an empty string.
    * @param a_form Pointer to the form to querry. Must NOT be nullptr.
    * @return The form's EditorID. Empty if the form doesn't have an EditorID or PO3's Tweaks are not installed AND the form's EditorID is not cached.
    */
    inline std::string GetEditorID(const RE::TESForm *a_form)
    {
        switch (a_form->GetFormType())
        {
        case RE::FormType::Keyword:
        case RE::FormType::LocationRefType:
        case RE::FormType::Action:
        case RE::FormType::MenuIcon:
        case RE::FormType::Global:
        case RE::FormType::HeadPart:
        case RE::FormType::Race:
        case RE::FormType::Sound:
        case RE::FormType::Script:
        case RE::FormType::Navigation:
        case RE::FormType::Cell:
        case RE::FormType::WorldSpace:
        case RE::FormType::Land:
        case RE::FormType::NavMesh:
        case RE::FormType::Dialogue:
        case RE::FormType::Quest:
        case RE::FormType::Idle:
        case RE::FormType::AnimatedObject:
        case RE::FormType::ImageAdapter:
        case RE::FormType::VoiceType:
        case RE::FormType::Ragdoll:
        case RE::FormType::DefaultObject:
        case RE::FormType::MusicType:
        case RE::FormType::StoryManagerBranchNode:
        case RE::FormType::StoryManagerQuestNode:
        case RE::FormType::StoryManagerEventNode:
        case RE::FormType::SoundRecord:
            return a_form->GetFormEditorID();
        default:
        {
            static auto tweaks = REX::W32::GetModuleHandleA("po3_Tweaks");
            static auto func = reinterpret_cast<_GetFormEditorID>(REX::W32::GetProcAddress(tweaks, "GetFormEditorID"));
            if (func)
            {
                return func(a_form->formID);
            }
            return {};
        }
        }
    }
}

namespace Hooks {
    struct ProjectileHook {
        static void Install() {
            REL::Relocation<std::uintptr_t> projVTABLE{ RE::VTABLE_GrenadeProjectile[0] };
            func = projVTABLE.write_vfunc(0xAC, &ProcessImpactHook);
            logs::info("Installing ProjectileHook");
        };

        static void ProcessImpactHook(RE::GrenadeProjectile* a_this) {
			RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
            auto player_loc = player->GetParentCell() != nullptr ? player->GetSaveParentCell() : nullptr;
			auto cell = a_this->parentCell;

			if (cell && cell != player_loc) {
				logs::info("ProjectileHook: Projectile {} is not in the player's location ({}), deleting projectile", EDID::GetEditorID(a_this), EDID::GetEditorID(player_loc));
                RE::GarbageCollector::GetSingleton()->Add(a_this, true);
			}
			func(a_this);
        }       
 
	private:
		static inline REL::Relocation<decltype(ProcessImpactHook)> func;
	};
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);
	Hooks::ProjectileHook::Install();
	return true;
}