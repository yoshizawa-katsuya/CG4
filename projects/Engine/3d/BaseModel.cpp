#include "BaseModel.h"
#include "Matrix.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include <numbers>
#include "ModelPlatform.h"
#include "Camera.h"
#include "Animation.h"
#include "Struct.h"

BaseModel::BaseModel()
	: modelPlatform_(ModelPlatform::GetInstance())
{
}

void BaseModel::CreateModel(const std::string& directoryPath, const std::string& filename) {

	//モデル読み込み
	LoadModelFile(directoryPath, filename);

	CreateVertexData();
	
	CreateIndexData();

	CreateMaterialData();

	//CreateTransformData();

	textureHandle_ = TextureManager::GetInstance()->Load(modelData_.material.textureFilePath);

}

void BaseModel::Draw(const WorldTransform& worldTransform, Camera* camera) {

	Matrix4x4 worldViewProjectionMatrix;
	if (camera) {
		const Matrix4x4& viewProjectionMatrix = camera->GetViewProjection();
		worldViewProjectionMatrix = Multiply(worldTransform.worldMatrix_, viewProjectionMatrix);
	}
	else {
		worldViewProjectionMatrix = worldTransform.worldMatrix_;
	}
	/*
	transformationMatrixData_->WVP = worldViewProjectionMatrix;
	transformationMatrixData_->World = worldMatrix;

	//wvp用のCBufferの場所を設定
	modelPlatform_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
	*/

	modelPlatform_->ModelDraw(worldViewProjectionMatrix, worldTransform.worldMatrix_, camera);

	if (isSkinModel_) {

		std::array<D3D12_VERTEX_BUFFER_VIEW, 2> vbvs = {
			vertexBufferView_,
			skinCluster_.influenceBufferView
		};

		modelPlatform_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 2, &vbvs.front());	//VBVを設定

		modelPlatform_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(4, skinCluster_.paletteSrvHandle.second);

	}
	else {

		modelPlatform_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);	//VBVを設定

	}

	modelPlatform_->GetDxCommon()->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	//マテリアルのCBufferの場所を設定
	modelPlatform_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(textureHandle_);

	//描画1(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
		//commandList_->DrawIndexedInstanced((kSubdivision * kSubdivision * 6), 1, 0, 0, 0);
	//modelPlatform_->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
	modelPlatform_->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indeces.size()), 1, 0, 0, 0);


}

void BaseModel::Draw(const WorldTransform& worldTransform, Camera* camera, uint32_t textureHandle)
{

	Matrix4x4 worldViewProjectionMatrix;
	if (camera) {
		const Matrix4x4& viewProjectionMatrix = camera->GetViewProjection();
		worldViewProjectionMatrix = Multiply(worldTransform.worldMatrix_, viewProjectionMatrix);
	}
	else {
		worldViewProjectionMatrix = worldTransform.worldMatrix_;
	}
	/*
	transformationMatrixData_->WVP = worldViewProjectionMatrix;
	transformationMatrixData_->World = worldMatrix;

	//wvp用のCBufferの場所を設定
	modelPlatform_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
	*/

	modelPlatform_->ModelDraw(worldViewProjectionMatrix, worldTransform.worldMatrix_, camera);

	if (isSkinModel_) {

		std::array<D3D12_VERTEX_BUFFER_VIEW, 2> vbvs = {
			vertexBufferView_,
			skinCluster_.influenceBufferView
		};

		modelPlatform_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 2, &vbvs.front());	//VBVを設定

		modelPlatform_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(4, skinCluster_.paletteSrvHandle.second);

	}
	else {

		modelPlatform_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);	//VBVを設定

	}

	modelPlatform_->GetDxCommon()->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	//マテリアルのCBufferの場所を設定
	modelPlatform_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(textureHandle);

	//描画1(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
		//commandList_->DrawIndexedInstanced((kSubdivision * kSubdivision * 6), 1, 0, 0, 0);
	//modelPlatform_->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
	modelPlatform_->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indeces.size()), 1, 0, 0, 0);

}

void BaseModel::CreateVertexData()
{

	//VertexResourceを生成
	vertexResource_ = modelPlatform_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());

	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点数分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	//頂点データをリソースにコピー
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());


}

void BaseModel::CreateIndexData()
{

	indexResource_ = modelPlatform_->GetDxCommon()->CreateBufferResource(sizeof(uint32_t) * modelData_.indeces.size());

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * modelData_.indeces.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	std::memcpy(indexData_, modelData_.indeces.data(), sizeof(uint32_t) * modelData_.indeces.size());

}

void BaseModel::CreateMaterialData()
{

	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource_ = modelPlatform_->GetDxCommon()->CreateBufferResource(sizeof(Material));
	//マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//白を書き込む
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentity4x4();

}
/*
void Model::CreateTransformData()
{

	//transformationMatrixのリソースを作る
	transformationMatrixResource_ = modelPlatform_->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));
	//データを書き込む
	//書き込むためのアドレスを取得
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
	//単位行列を書き込んでおく
	transformationMatrixData_->WVP = MakeIdentity4x4();

}
*/
void BaseModel::LoadModelFile(const std::string& directoryPath, const std::string& filename) {

	Assimp::Importer importer;
	std::string filepath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filepath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);

	assert(scene->HasMeshes());	//メッシュがないのは対応しない

	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());	//法線がないメッシュは今回は非対応
		assert(mesh->HasTextureCoords(0));	//TexcoordがないMeshは今回は非対応
		modelData_.vertices.resize(mesh->mNumVertices);	//最初に頂点数分のメモリを確保しておく
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcord = mesh->mTextureCoords[0][vertexIndex];
			//右手系->左手系の変換
			modelData_.vertices[vertexIndex].position = { -position.x, position.y, position.z, 1.0f };
			modelData_.vertices[vertexIndex].normal = { -normal.x, normal.y, normal.z };
			modelData_.vertices[vertexIndex].texcoord = { texcord.x, texcord.y };
		}

		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				modelData_.indeces.push_back(vertexIndex);
			}
		}

		//SkinCluster構築用のデータを取得
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData_.skinClusterData[jointName];

			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();	//BindPoseMatrixに戻す
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);	//成分を抽出
			//左手系のBindPoseMatrixを作る
			Matrix4x4 bindPoseMatrix = MakeAffineMatrix(
				{scale.x, scale.y, scale.z}, { -rotate.x, rotate.y, rotate.z, rotate.w }, { -translate.x, translate.y, translate.z });
			//InverseBindPoseMatrixにする
			jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);

			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId });
			}

		}

	}

	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData_.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
		}
		else {
			modelData_.material.textureFilePath = "./resources/white.png";
		}
	}

	modelData_.rootNode = ReadNode(scene->mRootNode);

	/*
	//1.中で必要となる変数の宣言
	
	std::vector<Vector4> positions;	//位置
	std::vector<Vector3> normals;	//法線
	std::vector<Vector2> texcords;	//テクスチャ座標
	std::string line;	//ファイルから読んだ1行を格納するもの

	//2.ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);	//ファイルを開く
	assert(file.is_open());	//とりあえず開けなかったら止める

	//3.実際にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;	//先頭の識別子を読む

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcord;
			s >> texcord.x >> texcord.y;
			texcord.y = 1.0f - texcord.y;
			texcords.push_back(texcord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3];
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点への要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');	//区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcord = texcords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				//VertexData vertex = { position, texcord, normal };
				//modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position, texcord, normal };
			}
			modelData_.vertices.push_back(triangle[2]);
			modelData_.vertices.push_back(triangle[1]);
			modelData_.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			LoadMaterialTemplateFile(directoryPath, materialFilename);
		}

	}
	*/
	
}

void BaseModel::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {

	//1.中で必要となる変数の宣言
	std::string line;	//ファイルから読んだ1行を格納するもの

	//2.ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);	//ファイルを開く
	assert(file.is_open());	//とりあえず開けなかったら止める

	//3.実際にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			modelData_.material.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}

	
}

Node BaseModel::ReadNode(aiNode* node)
{
	Node result;
	/*
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;	//nodeのlocalMatrixを取得
	aiLocalMatrix.Transpose();	//列ベクトルを行ベクトル形式に転置
	for (uint32_t i = 0; i < 4; i++) {
		for (uint32_t j = 0; j < 4; j++) {
			result.localMatrix.m[i][j] = aiLocalMatrix[i][j];	//他の要素も同様に
		}
	}
	*/

	aiVector3D scale, translate;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate);	//assimpの行列からSRTを抽出する関数を利用
	result.transform.scale = { scale.x, scale.y, scale.z };	//Scaleはそのまま
	result.transform.rotation = { -rotate.x, -rotate.y, -rotate.z, rotate.w };	//x軸を反転、さらに回転方向が逆なので軸を反転させる
	result.transform.translation = { -translate.x, translate.y, translate.z };	//x軸を反転
	result.localMatrix = MakeAffineMatrix(result.transform.scale, result.transform.rotation, result.transform.translation);

	result.name = node->mName.C_Str();	//Node名を格納
	result.children.resize(node->mNumChildren);	//子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		//再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}

void BaseModel::CreateSkelton()
{

	skeleton_.root = CreateJoint(modelData_.rootNode, {});

	//名前とindexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : skeleton_.joints) {
		skeleton_.jointMap.emplace(joint.name, joint.index);
	}

	SkeletonUpdate();

}

int32_t BaseModel::CreateJoint(const Node& node, const std::optional<int32_t>& parent)
{
	
	Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = MakeIdentity4x4();
	joint.transform = node.transform;
	joint.index = int32_t(skeleton_.joints.size());	//現在登録されている数をIndexに
	joint.parent = parent;
	skeleton_.joints.push_back(joint);	//SkeletonのJoints列に追加
	for (const Node& child : node.children) {
		//子Jointを作成し、そのIndexを登録
		int32_t childIndex = CreateJoint(child, joint.index);
		skeleton_.joints[joint.index].children.push_back(childIndex);
	}
	//自身のIndexを返す
	return joint.index;
}

void BaseModel::SkinClusterUpdate()
{

}