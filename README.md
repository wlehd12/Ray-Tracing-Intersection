# Ray-Tracing-Intersection

Mesh.obj에 배열로 되어있는 데이터를 받아 세개의 벡터로 triangle로 만들어서 형태를 나타낸다.

Ray 가 Mesh를 뚫고 지나가게 된다는 것은 Mesh구성하는 triangle을 뚫고 지나가는 것과 같다.

Triangle과 Ray의 충돌을 알아보기 위해서는 ray의 Normal 방향벡터 , triangle의 세 점 , triangle을 이루는 평면의 법선 벡터

![Untitled](https://github.com/wlehd12/Ray-Tracing-Intersection/assets/125344095/acc2130d-2a70-4456-ad79-54103a06d52b)

![Untitled](https://github.com/wlehd12/Ray-Tracing-Intersection/assets/125344095/aa893b33-6db4-44de-a3b4-d1c377b039dc)


#### 외적(Cross product)

b x a = -a x b

![image](https://github.com/wlehd12/Ray-Tracing-Intersection/assets/125344095/bd2ce110-ad2e-4c95-bab4-de9333e292ec)

![image](https://github.com/wlehd12/Ray-Tracing-Intersection/assets/125344095/7dc2ec99-f660-442a-9746-09dc5ca786d0)

#### 내적(Dot Product)

![image](https://github.com/wlehd12/Ray-Tracing-Intersection/assets/125344095/dc4da917-e30b-44ab-9049-f4c40a9abeb3)

---

triangle내부에 있을 경우세가지 경우의 외적 값이 triangl의 법선벡터와 같은 방향이다. 

Triangle외부에 있을 경우위와 같은 방식으로 계산하면
    외적의 오른손 법칙에 의해 반대 방향을      나타내는 값이 생긴다.

![image](https://github.com/wlehd12/Ray-Tracing-Intersection/assets/125344095/756d1e28-e71e-4b70-a3a5-0eb401ea2424)


#### Ray 최소화

3차원 두 벡터 사이의 거리 구하는 공식

∣AB∣=√ ((x2​−x1​) ²+(y2​−y1​) ²+(z2​−z1​) ²)




