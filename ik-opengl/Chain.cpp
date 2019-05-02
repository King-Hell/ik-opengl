//
//  Chain.cpp
//  ik-opengl
//
//  Created by Jesse Zhou on 6/2/17.
//  Copyright © 2017 Jesse and Robb. All rights reserved.
//

#include "Chain.h"

Chain::Chain(vector<glm::vec3> joints, Target *t) {

    vector<float> lengths;
    vector<glm::quat> directions;
    CalculateLinks(joints, &lengths, &directions);

    for (int i = 0; i < lengths.size(); ++i) {
        segments.push_back(Segment(joints[i], joints[i + 1], lengths[i], directions[i]));
        total_length += lengths[i];
    }

    target = t;
    size = joints.size();
    this->joints = joints;

}

Chain::Chain(glm::vec3 origin, glm::vec3 end, Target *t, int partitions) {

    vector<float> lengths;
    vector<glm::quat> directions;
    vector<glm::vec3> joints;

    glm::vec3 partition_checkpoint = origin;
    glm::vec3 dir = glm::normalize(end - origin);
    float magnitude = glm::length(end - origin);
    float partial_mag = magnitude / (float) partitions;

    joints.push_back(origin);
    for (int i = 1; i <= partitions; ++i) {
        glm::vec3 to_insert = partition_checkpoint + partial_mag * i * dir;
        joints.push_back(to_insert);
    }

    CalculateLinks(joints, &lengths, &directions);

    for (int i = 0; i < lengths.size(); ++i) {
        segments.push_back(Segment(joints[i], joints[i + 1], lengths[i], directions[i]));
        total_length += lengths[i];
    }

    target = t;
    size = joints.size();
    this->joints = joints;

}

void Chain::Solve() {
    /*计算IK*/
    /*1.首先，针对一条从首端到尾端的bone chain，以及一个对应的Target point，
     * 先计算整条chain的长度，从而判断这个Target point是否可到达。
     */
    float current_distance = glm::length(target->position - origin);

    // Target is out of reach; fully extend the arm
    if (current_distance > total_length) {
        for (int i = 0; i < joints.size() - 1; ++i) {
            float r = glm::length(target->position - joints[i]);
            float l = segments[i].magnitude / r;
            joints[i + 1] = (1 - l) * joints[i] + l * target->position;
        }

        vector<float> lengths;
        vector<glm::quat> directions;
        CalculateLinks(joints, &lengths, &directions);

    } else {
        /*2.如果整条骨骼链可以保证够得着的话，那么就可以开始进行FABRIK的处理。
         * 整体分为两步。
         */
        int count = 0;
        float difference = glm::length(joints[joints.size() - 1] - target->position);
        while (difference > tolerance || count < 1) {
            /*3.首先，先从末端骨骼开始计算，先将最末端的骨骼p4移到目标位置t处，
             *此时骨骼p4的位置为p'4。然后，将p3和p′4连成一条直线，通过原有的p3和p4的距离，
             *将现在的p3拉到与p′4同样的距离处p′3。*/
            Backward();
            /*4.再之后，再从根骨骼p′0开始处理。由于根骨骼再整个迭代过程中是默认为不动的，
             * 因此再把根骨骼p′′0移到原来的位置p0处，接着使用同样的距离约束，一直处理到尾骨骼p4。
             */
            Forward();
            /*重复3~4的迭代过程，直到最终的尾骨骼位置与到达目标位置（或与目标位置距离小于某个预定值）停止。
             * 此时整个算法结束。*/
            difference = glm::length(joints[joints.size() - 1] - target->position);
            count++;
            if (count > 10) break;
        }
    }
    SetSegments();

}

void Chain::SetSegments() {
    vector<float> lengths;
    vector<glm::quat> directions;
    CalculateLinks(joints, &lengths, &directions);

    for (int i = 0; i < lengths.size(); ++i) {
        segments[i].Set(joints[i], joints[i + 1], lengths[i], directions[i]);
    }
}

void Chain::Backward() {
    /*后向计算*/
    auto end = joints.end() - 1;
    *end = target->position;
    for (int i = int(joints.size() - 2); i >= 0; --i) {
        float r = glm::length(joints[i + 1] - joints[i]);
        float l = segments[i].magnitude / r;
        joints[i] = (1 - l) * joints[i + 1] + l * joints[i];
    }
}

void Chain::Forward() {
    auto beg = joints.begin();
    *beg = origin;
    for (int i = 0; i < joints.size() - 1; ++i) {
        float r = glm::length(joints[i + 1] - joints[i]);
        float l = segments[i].magnitude / r;

        glm::vec3 new_point = (1 - l) * joints[i] + l * joints[i + 1];
        if (i > 0 && this->please_constrain)
            new_point = Constrain(new_point, segments[i].magnitude, &(segments[i - 1]));
        joints[i + 1] = new_point;
    }
}

void Chain::CalculateLinks(vector<glm::vec3> joints, vector<float> *lengths, vector<glm::quat> *directions) {

    origin = *joints.begin();
    end = *(joints.end() - 1);

    for (auto it = joints.begin(); it != joints.end() - 1; ++it) {

        glm::vec3 current = *it;
        glm::vec3 next = *(it + 1);
        //计算骨骼长度
        glm::vec3 link_vector = next - current;

        lengths->push_back(glm::length(link_vector));

        //计算骨骼方向
        glm::vec3 cross = glm::cross(ref_rot_vector, link_vector);
        glm::quat q;
        q.x = cross.x;
        q.y = cross.y;
        q.z = cross.z;
        q.w = sqrt(pow(glm::length(link_vector), 2) * pow(glm::length(ref_rot_vector), 2)) +
              glm::dot(link_vector, ref_rot_vector);
        directions->push_back(glm::normalize(q));
    }
}

glm::vec3 Chain::Constrain(glm::vec3 point, float true_length, Segment *seg) {

    glm::vec3 retval = point;
    glm::vec3 relative_point = point - seg->end_position;
    bool debug = true;

    // Get projection of the new calculated point onto the direction the previous segment is pointing in
    glm::vec3 line_dir = glm::normalize(seg->GetConstraintConeAxis());
    float scalar = glm::dot(point - seg->end_position, line_dir);
    glm::vec3 projected_point = scalar * line_dir + seg->end_position;


    if (debug) {
        float angle = glm::acos(scalar / (glm::length(point - seg->end_position) * glm::length(line_dir)));
        cout << "angle: " << glm::degrees(angle) << endl;
        cout << "scalar: " << scalar << endl;
        cout << "projected point: " << projected_point.x << " " << projected_point.y << " " << projected_point.z << " "
             << endl;
    }

    glm::vec3 adjusted_distance = point - projected_point;
    if (scalar < 0) {
        glm::vec3 relative_projected_point = projected_point - seg->end_position;
        relative_projected_point = -relative_projected_point;
        projected_point = relative_projected_point + seg->end_position;
    }

    // Get the 2D axes we will be using for this problem
    glm::mat4 face_normals = seg->GetFaceNormals();
    glm::vec3 up = glm::normalize(glm::vec3(face_normals[0]));
    glm::vec3 down = glm::normalize(glm::vec3(face_normals[1]));
    glm::vec3 left = glm::normalize(glm::vec3(face_normals[2]));
    glm::vec3 right = glm::normalize(glm::vec3(face_normals[3]));

    glm::vec3 x_axis = glm::length(left - relative_point) < glm::length(right - relative_point) ? left : right;
    glm::vec3 y_axis = glm::length(up - relative_point) < glm::length(down - relative_point) ? up : down;

    float x_aspect = glm::dot(adjusted_distance, x_axis);
    float y_aspect = glm::dot(adjusted_distance, y_axis);

    if (debug) {
        cout << glm::radians(seg->constraint_cone[0]) << endl;
        cout << glm::radians(seg->constraint_cone[0]) << endl;
        cout << glm::tan(glm::radians(seg->constraint_cone[0])) << endl;
        cout << "x axis: " << x_axis.x << " " << x_axis.y << " " << x_axis.z << endl;
    }

    // Calculate the cone cross section
    float proj_length = glm::length(projected_point - seg->end_position);
    float up_cross = proj_length * glm::tan(glm::radians(seg->constraint_cone[0]));
    float down_cross = -(proj_length * glm::tan(glm::radians(seg->constraint_cone[1])));
    float left_cross = -(proj_length * glm::tan(glm::radians(seg->constraint_cone[2])));
    float right_cross = proj_length * glm::tan(glm::radians(seg->constraint_cone[3]));

    // See which quadrant we should use
    float x_bound = x_aspect >= 0 ? right_cross : left_cross;
    float y_bound = y_aspect >= 0 ? up_cross : down_cross;

    // Calculate where our point lies on the ellipse (finally)
    float ellipse_point = glm::pow(x_aspect, 2) / glm::pow(x_bound, 2) + glm::pow(y_aspect, 2) / glm::pow(y_bound, 2);

    //if(debug) cout << "ellipse point: " << ellipse_point << endl;
    cout << "y_aspect " << y_aspect << "  x_aspect " << x_bound << endl;

    // If the point we calculated is outside of this ellipse, then we must constrain the joint
    if (ellipse_point > 1 || scalar < 0) {
        cout << "Not in bounds!" << endl;
        float a = glm::atan(y_aspect, x_aspect);
        float x = x_bound * glm::cos(a);
        float y = y_bound * glm::sin(a);

        retval = glm::normalize((projected_point - seg->end_position) + (x_axis * x) + (y_axis * y)) *
                 glm::length(relative_point) + seg->end_position;
    } else {
        cout << "In bounds!" << endl;
    }

    return retval;
}

void Chain::Render(glm::mat4 view, glm::mat4 proj) {
    /*渲染方法*/
    for (auto it = segments.begin(); it != segments.end(); ++it) {
        it->Render(view, proj);
    }
}

glm::vec3 Chain::GetFirstJoint() {
    /*获取第一个关节*/
    return joints[0];
}

void Chain::SetFirstJoint(glm::vec3 joint) {
    /*设置第一个关节*/
    joints[0] = joint;
}
