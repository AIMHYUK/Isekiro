# Isekiro

Unreal Engine 5.6으로 제작한 세키로(Sekiro: Shadows Die Twice) 스타일의 3인칭 액션 게임 프로젝트입니다.

## 시연 영상

[![Isekiro Demo](https://img.youtube.com/vi/BzzkD_gu2F0/0.jpg)](https://youtu.be/BzzkD_gu2F0)

## 개요

플레이어가 사무라이 캐릭터를 조종하며 보스와 1:1 전투를 펼치는 게임입니다. 세키로의 핵심 전투 메커니즘인 **패리(Parry)**, **자세 게이지(Posture)**, **처형(Execution)** 시스템을 직접 구현한 것이 특징입니다.

## 주요 기술

- **엔진**: Unreal Engine 5.6
- **언어**: C++
- **렌더링 이펙트**: Niagara Fluids
- **UI**: UMG (Unreal Motion Graphics)

## 핵심 구현 내용

### 보스 AI - FSM (Finite State Machine)

보스 AI는 컴포넌트 기반의 FSM으로 설계되었습니다.

- `UFSMComponent` — 보스의 행동 상태를 관리하는 핵심 컴포넌트
- `UStateObject` — 각 상태(State)의 기반 클래스 (Start / End / Interrupt 인터페이스)
- 보스와 플레이어 사이의 거리에 따라 **근거리(NEAR) / 원거리(FAR)** 공간을 분리하여 상태를 선택

**보스 상태 목록**

| 카테고리 | 상태 |
|---|---|
| 반응 (Stun) | Hit, Block, Parry, Deflected |
| 근거리 공격 | NormalAttack, CounterAttack, Strafe |
| 원거리 공격 | DistanceAttack, ThrustAttack, Run, PatternAttack, LungeAttack |
| 전환 | Dodge, DodgeAttack, Jump, JumpStrike, JumpFire |
| 기타 | Death |

**확률 기반 행동 시스템**
- 플레이어가 공격할수록 보스의 **회피(Dodge) 확률**이 점진적으로 증가
- 방어(Block) / 패리(Parry) 확률을 별도로 설정 가능

### 타겟 락온 시스템 (Target Lock-on System)

> `Component-based` · `Vector Dot Product` · `Modular Design`

<!-- GIF: 락온 시연 -->
<!-- ![Target Lock-on](docs/gifs/lockon.gif) -->

**[Core Mechanics]**

- **Component-based Design**: `UActorComponent`를 활용하여 모든 플레이어 캐릭터에 즉시 탑재 가능한 모듈형 락온 시스템 구현.
- **Optimal Target Scoring**: 내적(Dot Product)을 활용해 카메라 정면 벡터와 타겟 벡터 간의 유사도를 측정, 화면 중앙에 가장 가까운 적을 우선적으로 선택하는 알고리즘 설계.
- **Dynamic View Control**: `RInterp To`를 이용해 타겟 추적 시 카메라 회전의 부드러운 보간 처리 및 락온 상태에 따른 캐릭터 이동 방식(Strafing) 자동 전환.
- **Obstacle Awareness**: Line Trace를 통한 시야 검사(Line of Sight)를 수행하여 장애물 뒤의 적은 타겟 대상에서 제외.

---

### 그래플링 훅 시스템 (Grappling Hook System)

> `Physics-based` · `Predictive Targeting` · `State Synchronization`

<!-- GIF: 그래플링 훅 시연 -->
<!-- ![Grappling Hook](docs/gifs/hook.gif) -->

**[Core Mechanics]**

- **Predictive Landing Logic**: 타겟 지점 감지 후 하단 트레이스를 추가 수행하여 캐릭터가 실제 착지 가능한 지형(Walkable Surface)인지 사전에 검증.
- **Smooth Motion Profiling**: Timeline과 Curve를 결합하여 발사 시 가속도와 도착 시 감속도를 조절, 물리적으로 자연스러운 이동 궤적 구현.
- **Cable Physics Integration**: 언리얼 엔진의 Cable Component를 활용해 발사체와 캐릭터 손(Socket) 사이의 물리적 연결 및 환경 충돌 구현.
- **Anim-driven State Control**: 애니메이션 노티파이(Anim Notify)를 활용해 이동 로직의 시작/종료 시점과 캐릭터의 Flying/Falling 상태를 정밀하게 동기화.

---

### 전투 시스템

- **콤보 공격**: 입력 타이밍에 따라 연속 공격 가능
- **패리 & 자세 게이지**: 성공적인 패리 시 보스의 자세 게이지 누적, 자세가 무너지면 처형 가능
- **처형(Execution)**: 보스의 자세(Posture)가 완전히 무너졌을 때 처형 공격 실행
- **화살(Arrow)**: 보스의 원거리 공격. Niagara 트레일 이펙트 포함

### 캐릭터 구조

```
ABaseCharacter
├── AHeroCharacter     (플레이어)
└── ABossCharacter     (보스)
```

- `UStatusComponent` — HP 등 캐릭터 공통 스탯 관리
- `IDamageInterface` — 데미지 처리 공통 인터페이스

## 프로젝트 구조

```
Source/Isekiro/
├── Public/
│   ├── Character/         # 캐릭터 클래스 (Base, Hero, Boss)
│   ├── FSM/               # 보스 FSM 컴포넌트 및 상태 클래스
│   │   └── BossStates/    # 각 보스 상태 구현체
│   ├── ActorComponents/   # StatusComponent 등 공용 컴포넌트
│   ├── Arrow.h            # 보스 원거리 투사체
│   └── ...                # UI, AnimInstance, PlayerController 등
└── Private/               # 구현 파일 (.cpp)
```

## 빌드 방법

1. Unreal Engine 5.6 설치
2. `Isekiro.uproject` 파일을 우클릭 → **Generate Visual Studio project files** 선택
3. `Isekiro.sln`을 Visual Studio로 열어 빌드
4. 또는 `Isekiro.uproject`를 Unreal Editor로 직접 열기
