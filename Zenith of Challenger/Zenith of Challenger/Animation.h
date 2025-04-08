#pragma once
#include"stdafx.h"

struct Keyframe
{
    float time;
    XMFLOAT3 position;
    XMFLOAT4 rotation;
    XMFLOAT3 scale;
};

struct BoneAnimation
{
    string boneName;
    vector<Keyframe> keyframes;
};

struct AnimationClip
{
    string name;
    float duration;         // ��ü ��� �ð� (ticks)
    float ticksPerSecond;   // �ʴ� ƽ ��

    unordered_map<string, BoneAnimation> boneAnimations; // boneName �� �ִϸ��̼� ����

    DirectX::XMMATRIX GetBoneTransform(const BoneAnimation& boneAnim, float time) const;
    unordered_map<string, DirectX::XMMATRIX> GetBoneTransforms(float time) const;
};

DirectX::XMMATRIX InterpolateKeyframes(const Keyframe& a, const Keyframe& b, float t);