using UnrealBuildTool;

public class U0Editor: ModuleRules
{
    public U0Editor(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "EditorFramework",
                "UnrealEd",
                "U0Game",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Slate",
                "SlateCore",
                "ToolMenus",
                "MessageLog",
            }
        );
    }
}
