// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ModularGameFrameworkEditorTarget : TargetRules
{
	public ModularGameFrameworkEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

        ExtraModuleNames.Add("ModularGameFramework" /* ,"ModularGameFrameworkEditor" */);

        if (!bBuildAllModules)
        {
            NativePointerMemberBehaviorOverride = PointerMemberBehavior.Disallow;
        }

        ModularGameFrameworkTarget.ApplySharedModularGameFrameworkTargetSettings(this);

        // This is used for touch screen development along with the "Unreal Remote 2" app
        EnablePlugins.Add("RemoteSession");

        ProjectDefinitions.Add("MODIO_DEVELOPMENT_MODE");
    }
}
