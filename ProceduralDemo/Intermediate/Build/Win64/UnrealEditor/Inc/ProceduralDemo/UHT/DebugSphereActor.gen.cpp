// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "ProceduralDemo/DebugSphereActor.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeDebugSphereActor() {}
// Cross Module References
	ENGINE_API UClass* Z_Construct_UClass_AActor();
	PROCEDURALDEMO_API UClass* Z_Construct_UClass_ADebugSphereActor();
	PROCEDURALDEMO_API UClass* Z_Construct_UClass_ADebugSphereActor_NoRegister();
	UPackage* Z_Construct_UPackage__Script_ProceduralDemo();
// End Cross Module References
	void ADebugSphereActor::StaticRegisterNativesADebugSphereActor()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ADebugSphereActor);
	UClass* Z_Construct_UClass_ADebugSphereActor_NoRegister()
	{
		return ADebugSphereActor::StaticClass();
	}
	struct Z_Construct_UClass_ADebugSphereActor_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_ADebugSphereActor_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AActor,
		(UObject* (*)())Z_Construct_UPackage__Script_ProceduralDemo,
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ADebugSphereActor_Statics::DependentSingletons) < 16);
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_ADebugSphereActor_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "DebugSphereActor.h" },
		{ "ModuleRelativePath", "DebugSphereActor.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_ADebugSphereActor_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ADebugSphereActor>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_ADebugSphereActor_Statics::ClassParams = {
		&ADebugSphereActor::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x009000A4u,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ADebugSphereActor_Statics::Class_MetaDataParams), Z_Construct_UClass_ADebugSphereActor_Statics::Class_MetaDataParams)
	};
	UClass* Z_Construct_UClass_ADebugSphereActor()
	{
		if (!Z_Registration_Info_UClass_ADebugSphereActor.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ADebugSphereActor.OuterSingleton, Z_Construct_UClass_ADebugSphereActor_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_ADebugSphereActor.OuterSingleton;
	}
	template<> PROCEDURALDEMO_API UClass* StaticClass<ADebugSphereActor>()
	{
		return ADebugSphereActor::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(ADebugSphereActor);
	ADebugSphereActor::~ADebugSphereActor() {}
	struct Z_CompiledInDeferFile_FID_Users_vojta_source_repos_ProceduralDemo_ProceduralDemo_Source_ProceduralDemo_DebugSphereActor_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_vojta_source_repos_ProceduralDemo_ProceduralDemo_Source_ProceduralDemo_DebugSphereActor_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_ADebugSphereActor, ADebugSphereActor::StaticClass, TEXT("ADebugSphereActor"), &Z_Registration_Info_UClass_ADebugSphereActor, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ADebugSphereActor), 2886286092U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_vojta_source_repos_ProceduralDemo_ProceduralDemo_Source_ProceduralDemo_DebugSphereActor_h_1924455676(TEXT("/Script/ProceduralDemo"),
		Z_CompiledInDeferFile_FID_Users_vojta_source_repos_ProceduralDemo_ProceduralDemo_Source_ProceduralDemo_DebugSphereActor_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_vojta_source_repos_ProceduralDemo_ProceduralDemo_Source_ProceduralDemo_DebugSphereActor_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
