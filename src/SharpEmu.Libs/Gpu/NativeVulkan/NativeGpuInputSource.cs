// Copyright (C) 2026 SharpEmu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

using System.Runtime.InteropServices;
using SharpEmu.HLE.Host;
using SharpEmu.HLE.Host.Posix;

namespace SharpEmu.Libs.Gpu.NativeVulkan;

internal sealed unsafe class NativeGpuInputSource : IPosixWindowInputSource
{
    internal static NativeGpuInputSource Instance { get; } = new();
    private readonly object _gate = new();
    private readonly uint[] _keys = new uint[8];
    private bool _focused;
    private bool _gamepadConnected;
    private HostGamepadState _gamepad;
    private string? _gamepadName;

    private NativeGpuInputSource() { }

    internal void Attach() => PosixHostInput.SetSource(this);

    internal void Update(NativeVulkanApi.Input* state)
    {
        lock (_gate)
        {
            _focused = state->KeyboardFocused != 0;
            for (var index = 0; index < _keys.Length; ++index) _keys[index] = state->VirtualKeys[index];
            _gamepadConnected = state->GamepadConnected != 0;
            _gamepad = new HostGamepadState(
                _gamepadConnected,
                (HostGamepadButtons)state->GamepadButtons,
                state->LeftX,
                state->LeftY,
                state->RightX,
                state->RightY,
                state->LeftTrigger,
                state->RightTrigger);
            _gamepadName = _gamepadConnected
                ? Marshal.PtrToStringUTF8((nint)state->GamepadNameUtf8)
                : null;
        }
    }

    public bool HasKeyboardFocus
    {
        get { lock (_gate) return _focused; }
    }

    public bool IsKeyDown(int virtualKey)
    {
        if ((uint)virtualKey >= 256) return false;
        lock (_gate) return (_keys[virtualKey / 32] & (1u << (virtualKey % 32))) != 0;
    }

    public int GetGamepadStates(Span<HostGamepadState> destination)
    {
        lock (_gate)
        {
            if (!_gamepadConnected || destination.IsEmpty) return 0;
            destination[0] = _gamepad;
            return 1;
        }
    }

    public string? DescribeConnectedGamepad()
    {
        lock (_gate) return _gamepadConnected ? _gamepadName ?? "SDL gamepad" : null;
    }
}
