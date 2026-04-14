# Isekiro

Unreal Engine 5.6으로 제작한 세키로(Sekiro: Shadows Die Twice) 스타일의 3인칭 액션 게임 프로젝트입니다.

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

### 전투 시스템

- **콤보 공격**: 입력 타이밍에 따라 연속 공격 가능
- **패리 & 자세 게이지**: 성공적인 패리 시 보스의 자세 게이지 누적, 자세가 무너지면 처형 가능
- **처형(Execution)**: 보스의 자세(Posture)가 완전히 무너졌을 때 처형 공격 실행
- **화살(Arrow)**: 보스의 원거리 공격. Niagara 트레일 이펙트 포함
- **락온(Target Lock-On)**: `TargetLockOnComponent`로 보스 고정 시점 지원

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
