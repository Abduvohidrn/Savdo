using UnrealBuildTool;

public class BozorUnreal : ModuleRules
{
	public BozorUnreal(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject", "Engine", "InputCore", "AIModule", "NavigationSystem",
			"AnimationCore", "GameplayTasks", "PhysicsCore", "UMG", "EnhancedInput"
		});
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
		}
	}
}
