// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeProceduralDemo_init() {}
	static FPackageRegistrationInfo Z_Registration_Info_UPackage__Script_ProceduralDemo;
	FORCENOINLINE UPackage* Z_Construct_UPackage__Script_ProceduralDemo()
	{
		if (!Z_Registration_Info_UPackage__Script_ProceduralDemo.OuterSingleton)
		{
			static const UECodeGen_Private::FPackageParams PackageParams = {
				"/Script/ProceduralDemo",
				nullptr,
				0,
				PKG_CompiledIn | 0x00000000,
				0x13A5BAE5,
				0xD247B820,
				METADATA_PARAMS(0, nullptr)
			};
			UECodeGen_Private::ConstructUPackage(Z_Registration_Info_UPackage__Script_ProceduralDemo.OuterSingleton, PackageParams);
		}
		return Z_Registration_Info_UPackage__Script_ProceduralDemo.OuterSingleton;
	}
	static FRegisterCompiledInInfo Z_CompiledInDeferPackage_UPackage__Script_ProceduralDemo(Z_Construct_UPackage__Script_ProceduralDemo, TEXT("/Script/ProceduralDemo"), Z_Registration_Info_UPackage__Script_ProceduralDemo, CONSTRUCT_RELOAD_VERSION_INFO(FPackageReloadVersionInfo, 0x13A5BAE5, 0xD247B820));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
